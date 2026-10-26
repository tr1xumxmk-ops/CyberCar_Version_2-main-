/*
  SPARK CAR // SPARK TEAM — NEON EDITION PRO V2
  Modular ESP32-S2 captive portal car controller.

  Open this .ino in Arduino IDE. Keep all .h files in this same sketch folder.
  Dashboard and behavior are embedded in captive_portal.h; original team face art is compressed and kept in face_assets.h (PROGMEM).

  Hardware/features preserved from the previous single-file edition:
    - ESP32-S2 creates a local Wi-Fi AP/captive portal (Spark-Car / 12345678).
    - L298N digital motor control; FAST is full continuous enable; MEDIUM/SLOW are timed digital bursts.
    - Emergency brake latch and hold-to-drive button timeout.
    - Manual ball-servo angle from 0..180, starts at 0 degrees.
    - Manual, timed AUTO demo, and Q-learning UI simulation mode.

  Required library: ESP32Servo

  Electrical safety:
    - Remove L298N ENA/ENB jumper caps when controlled by GPIO 11/12.
    - Motor driver needs its own suitable motor supply and common GND with ESP32.
    - Power the servo from a suitable regulated supply, not the 3V3 pin.
    - The portal uses HTTP and is intended for the local AP only.
    - Test with wheels raised and keep a physical battery disconnect accessible.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_task_wdt.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "config.h"
#include "control_types.h"
#include "motor_driver.h"
#include "accessories.h"
#include "car_controller.h"
#include "secure_backend.h"
#include "captive_portal.h"
#include "face_routes.h"
#include "web_routes.h"

// Global objects are defined exactly once here. Headers only declare externs.
CarController car;
DNSServer dnsServer;
WebServer webServer(80);
uint32_t lastDriveRequestAt = 0;
uint32_t lastServoRequestAt = 0;
uint32_t lastHornRequestAt = 0;
uint32_t lastSpeedRequestAt = 0;
uint32_t lastModeRequestAt = 0;
uint32_t lastStateRequestAt = 0;

static const char* resetReasonName(const esp_reset_reason_t reason) {
  switch (reason) {
    case ESP_RST_POWERON: return "power-on";
    case ESP_RST_EXT: return "external-reset";
    case ESP_RST_SW: return "software-reset";
    case ESP_RST_PANIC: return "panic/exception";
    case ESP_RST_INT_WDT: return "interrupt-watchdog";
    case ESP_RST_TASK_WDT: return "task-watchdog";
    case ESP_RST_WDT: return "watchdog";
    case ESP_RST_DEEPSLEEP: return "deep-sleep";
    case ESP_RST_BROWNOUT: return "brownout/low-supply-voltage";
    case ESP_RST_SDIO: return "SDIO";
    default: return "other/unknown";
  }
}

static void initializeWatchdog() {
  const esp_task_wdt_config_t watchdogConfig = {
      .timeout_ms = SparkConfig::WATCHDOG_TIMEOUT_MS,
      .idle_core_mask = (1U << portNUM_PROCESSORS) - 1U,
      .trigger_panic = true
  };

  esp_err_t result = esp_task_wdt_init(&watchdogConfig);
  if (result == ESP_ERR_INVALID_STATE) {
    result = esp_task_wdt_reconfigure(&watchdogConfig);
  }
  if (result != ESP_OK) {
    Serial.printf("[WARN] Watchdog configuration failed: %d\n", static_cast<int>(result));
  }

  const esp_err_t addResult = esp_task_wdt_add(nullptr);
  if (addResult != ESP_OK && addResult != ESP_ERR_INVALID_STATE) {
    Serial.printf("[WARN] Watchdog task registration failed: %d\n", static_cast<int>(addResult));
  }
}

void setup() {
  Serial.begin(SparkConfig::SERIAL_BAUD_RATE);
  delay(SparkConfig::STARTUP_SETTLE_MS);
  const esp_reset_reason_t resetReason = esp_reset_reason();
  Serial.printf("\n[BOOT] Reset reason: %s (%d)\n",
                resetReasonName(resetReason), static_cast<int>(resetReason));
  Serial.printf("[BOOT] Free heap: %lu bytes\n", static_cast<unsigned long>(ESP.getFreeHeap()));
  initializeWatchdog();
  WiFi.persistent(false);
  // Leave Wi-Fi sleep policy at the core default instead of explicitly disabling it.
  const IPAddress ip(SparkConfig::IP_A, SparkConfig::IP_B, SparkConfig::IP_C, SparkConfig::IP_D);
  const IPAddress mask(255, 255, 255, 0);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(ip, ip, mask);
  const bool apStarted = WiFi.softAP(SparkConfig::AP_SSID, SparkConfig::AP_PASSWORD,
                                     SparkConfig::AP_CHANNEL, false, SparkConfig::AP_MAX_CLIENTS);
  if (!apStarted) {
    Serial.println(F("[ERROR] Wi-Fi access point failed to start."));
  } else {
    // 72 quarter-dBm units = 18 dBm maximum TX power on ESP-IDF.
    // This is a conservative setting for clone boards; power quality still matters.
    const esp_err_t txPowerResult = esp_wifi_set_max_tx_power(72);
    if (txPowerResult == ESP_OK) Serial.println(F("[WIFI] Maximum TX power limited to 18 dBm."));
    else Serial.printf("[WARN] Could not set Wi-Fi TX power: %d\n", static_cast<int>(txPowerResult));
  }
  // Start servo/motor outputs after Wi-Fi setup to avoid adding both startup surges at once.
  car.begin();
  static const char* kHeaders[] = {"Origin"};
  webServer.collectHeaders(kHeaders, 1);
  webServer.on("/", HTTP_GET, routeIndex);
  webServer.on("/api/state", HTTP_GET, routeState);
  webServer.on("/api/drive", HTTP_POST, routeDrive);
  // The old /api/joy endpoint is no longer exposed; the dashboard uses named buttons.
  webServer.on("/api/speed", HTTP_POST, routeSpeed);
  webServer.on("/api/mode", HTTP_POST, routeMode);
  webServer.on("/api/servo", HTTP_POST, routeServo);
  webServer.on("/api/horn", HTTP_POST, routeHorn);
  webServer.on("/api/brake", HTTP_POST, routeBrake);
  webServer.on("/api/resume", HTTP_POST, routeResume);
  registerFaceAssetRoutes();
  webServer.on("/hotspot-detect.html", HTTP_GET, redirectToPortal);
  webServer.on("/generate_204", HTTP_GET, redirectToPortal);
  webServer.on("/connecttest.txt", HTTP_GET, redirectToPortal);
  webServer.on("/ncsi.txt", HTTP_GET, redirectToPortal);
  webServer.onNotFound(routeNotFound);
  webServer.begin();
  dnsServer.start(SparkConfig::DNS_PORT, "*", ip);

  Serial.println();
  Serial.println(F("=============================================="));
  Serial.println(F(" SPARK CAR // SPARK TEAM NEON EDITION"));
  Serial.printf(" Wi-Fi: %s\n", SparkConfig::AP_SSID);
  Serial.println(F(" Password: 12345678"));
  Serial.println(F(" Portal: http://192.168.4.1/"));
  Serial.println(F(" No web PIN; AP password protects local access."));
  Serial.println(F(" Motors: digital GPIO; FAST = full continuous enable"));
  Serial.println(F(" Ball servo: manual angle, initialized at 0 degrees"));
  Serial.println(F(" Physical RGB status LED outputs: removed/disabled"));
  Serial.println(F(" Vehicle control is local-only; do not expose HTTP publicly."));
  Serial.println(F("=============================================="));
}

void loop() {
  // Feed before and after network service so a stuck handler is caught by the watchdog.
  esp_task_wdt_reset();
  dnsServer.processNextRequest();
  webServer.handleClient();
  // Refresh time AFTER HTTP processing; handlers may update command timestamps.
  car.update(millis());
  esp_task_wdt_reset();
  delay(SparkConfig::MAIN_LOOP_YIELD_MS);
}
