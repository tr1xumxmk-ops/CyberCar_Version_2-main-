#ifndef SPARK_CAR_SECURE_BACKEND_H
#define SPARK_CAR_SECURE_BACKEND_H

#include <Arduino.h>
#include <WebServer.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include "config.h"
#include "control_types.h"

extern WebServer webServer;
extern uint32_t lastDriveRequestAt;
extern uint32_t lastServoRequestAt;
extern uint32_t lastHornRequestAt;
extern uint32_t lastSpeedRequestAt;
extern uint32_t lastModeRequestAt;
extern uint32_t lastStateRequestAt;

static void addSecurityHeaders(const bool cacheStaticAsset = false) {
  webServer.sendHeader("X-Content-Type-Options", "nosniff");
  webServer.sendHeader("X-Frame-Options", "DENY");
  webServer.sendHeader("Referrer-Policy", "no-referrer");
  webServer.sendHeader("Cache-Control", cacheStaticAsset ? "public, max-age=604800, immutable" : "no-store");
  webServer.sendHeader("Content-Security-Policy",
      "default-src 'self'; img-src 'self' data:; style-src 'self' 'unsafe-inline'; "
      "script-src 'self' 'unsafe-inline'; connect-src 'self'; object-src 'none'; "
      "base-uri 'none'; frame-ancestors 'none'");
}

static void sendJson(const int status, const char* body) {
  addSecurityHeaders();
  webServer.send(status, "application/json; charset=utf-8", body);
}

static bool parseBoundedInt(const char* name, const int minValue, const int maxValue, int& result) {
  if (!webServer.hasArg(name)) return false;
  const String value = webServer.arg(name);
  if (value.length() == 0U || value.length() > 6U) return false;

  const char* raw = value.c_str();
  const char* cursor = raw;
  if (*cursor == '-') ++cursor;
  if (*cursor == '\0') return false;
  for (; *cursor != '\0'; ++cursor) {
    if (*cursor < '0' || *cursor > '9') return false;
  }

  char* end = nullptr;
  const long parsed = strtol(raw, &end, 10);
  if (end == raw || *end != '\0' || parsed < minValue || parsed > maxValue) return false;
  result = static_cast<int>(parsed);
  return true;
}

// The Wi-Fi WPA2 password is the access gate. There is intentionally no extra web-page PIN.
// State-changing requests still must originate from this local portal, and values are validated.
static bool requireTrustedOrigin() {
  const String origin = webServer.header("Origin");
  if (origin == "http://192.168.4.1" || origin == "http://192.168.4.1:80") return true;
  sendJson(403, "{\"ok\":false,\"error\":\"origin_rejected\"}");
  return false;
}

static bool requestRateLimited(uint32_t& last, const uint32_t intervalMs) {
  const uint32_t now = millis();
  if (last != 0U && !elapsedMs(now, last, intervalMs)) return true;
  last = now;
  return false;
}

#endif // SPARK_CAR_SECURE_BACKEND_H
