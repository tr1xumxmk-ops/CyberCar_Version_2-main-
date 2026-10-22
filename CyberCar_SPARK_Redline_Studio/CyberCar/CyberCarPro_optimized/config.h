// CYBER CAR SPARK REDLINE — shared compile-time configuration
#pragma once
#include <Arduino.h>
#include <stdint.h>

namespace Config {
  // Wi-Fi access point. Change these before sharing or demonstrating the car.
  constexpr char AP_SSID[] = "Cyber-Car-Pro";
  constexpr char AP_PASSWORD[] = "NeonDrive_2026!";
  // Change this six-digit control PIN before public demonstrations or sharing firmware.
  // It is a second local access gate, not a substitute for HTTPS on public networks.
  constexpr char CONTROL_PIN[] = "583104";
  constexpr uint8_t MAX_LOGIN_FAILURES = 5;
  constexpr uint32_t LOGIN_LOCKOUT_MS = 30000;
  constexpr uint8_t AP_CHANNEL = 6;
  constexpr uint8_t AP_MAX_CLIENTS = 2;
  constexpr uint16_t DNS_PORT = 53;
  constexpr uint8_t IP_OCTET_1 = 192;
  constexpr uint8_t IP_OCTET_2 = 168;
  constexpr uint8_t IP_OCTET_3 = 4;
  constexpr uint8_t IP_OCTET_4 = 1;

  // L298N pins. ENA/ENB must be connected to these GPIOs and their jumper caps removed.
  constexpr uint8_t MOTOR_IN1_PIN = 7;
  constexpr uint8_t MOTOR_IN2_PIN = 6;
  constexpr uint8_t MOTOR_IN3_PIN = 4;
  constexpr uint8_t MOTOR_IN4_PIN = 5;
  constexpr uint8_t MOTOR_ENA_PIN = 11;
  constexpr uint8_t MOTOR_ENB_PIN = 12;
  constexpr bool INVERT_LEFT_MOTOR = false;
  constexpr bool INVERT_RIGHT_MOTOR = false;

  // Optional existing accessories.
  constexpr uint8_t BALL_SERVO_PIN = 13;
  constexpr uint8_t LED_RED_PIN = 8;
  constexpr uint8_t LED_GREEN_PIN = 9;
  constexpr uint8_t LED_BLUE_PIN = 3;
  constexpr uint8_t BUZZER_PIN = 18;
  constexpr uint8_t BALL_LOCKED_ANGLE = 0;
  constexpr uint8_t BALL_RELEASED_ANGLE = 90;
  constexpr uint16_t SERVO_MIN_US = 500;
  constexpr uint16_t SERVO_MAX_US = 2400;

  // Watchdog and network guardrails.
  constexpr uint16_t MANUAL_COMMAND_TIMEOUT_MS = 500;
  constexpr uint16_t JOYSTICK_RATE_LIMIT_MS = 45;
  constexpr uint16_t CONTROL_RATE_LIMIT_MS = 120;
  constexpr uint16_t STATE_RATE_LIMIT_MS = 250;
  constexpr uint32_t WATCHDOG_TIMEOUT_MS = 10000;
  constexpr uint32_t SESSION_MAX_AGE_SECONDS = 3600;
  constexpr uint8_t TOKEN_HEX_LENGTH = 32;

  // Coarse digital burst profiles. These use timed ON/OFF GPIO writes, not LEDC hardware PWM.
  // Slow/Medium deliberately switch ENA/ENB fully ON/OFF in a low-rate window.
  // Expect pulsing/jerky motion; use hardware PWM or a motor voltage controller for smooth speed.
  constexpr uint16_t DRIVE_WINDOW_MS = 200;
  constexpr uint16_t MEDIUM_ON_MS = 150;
  constexpr uint16_t SLOW_ON_MS = 95;

  // Battery telemetry is deliberately omitted: the pictured build has no confirmed voltage-divider wiring.
  constexpr uint16_t UI_REFRESH_MS = 1000;
}

enum class DriveCommand : uint8_t { Stop, Forward, Backward, Left, Right, Brake };
enum class DriveLevel : uint8_t { Slow, Medium, Fast };

static inline bool elapsed(const uint32_t now, const uint32_t since, const uint32_t interval) {
  return static_cast<uint32_t>(now - since) >= interval;
}
