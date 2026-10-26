#ifndef SPARK_CAR_CONFIG_H
#define SPARK_CAR_CONFIG_H

#include <Arduino.h>
#include <stdint.h>

// Centralized compile-time settings. Edit these values to customize the project.
namespace SparkConfig {
  // Runtime/platform constants.
  constexpr uint32_t SERIAL_BAUD_RATE = 115200UL;
  constexpr uint16_t STARTUP_SETTLE_MS = 100;
  constexpr uint16_t MAIN_LOOP_YIELD_MS = 2;

  // Network settings — customize before sharing.
  constexpr char AP_SSID[] = "Spark-Car";
  constexpr char AP_PASSWORD[] = "12345678";
  constexpr uint8_t AP_CHANNEL = 6;
  constexpr uint8_t AP_MAX_CLIENTS = 2;
  constexpr uint16_t DNS_PORT = 53;
  constexpr uint8_t IP_A = 192;
  constexpr uint8_t IP_B = 168;
  constexpr uint8_t IP_C = 4;
  constexpr uint8_t IP_D = 1;

  // L298N pins: IN1/IN2 are one motor; IN3/IN4 are the other.
  constexpr uint8_t MOTOR_IN1_PIN = 7;
  constexpr uint8_t MOTOR_IN2_PIN = 6;
  constexpr uint8_t MOTOR_IN3_PIN = 4;
  constexpr uint8_t MOTOR_IN4_PIN = 5;
  constexpr uint8_t MOTOR_ENA_PIN = 11;
  constexpr uint8_t MOTOR_ENB_PIN = 12;
  constexpr bool INVERT_LEFT_MOTOR = false;
  constexpr bool INVERT_RIGHT_MOTOR = false;

  // Optional accessories from the current project wiring.
  constexpr uint8_t BALL_SERVO_PIN = 13;
  // Existing buzzer signal pin. GPIO 18 must be connected to the buzzer input/signal.
  // A standard L298N board does not provide an onboard buzzer signal by itself.
  constexpr uint8_t BUZZER_PIN = 18;
  constexpr uint16_t SERVO_MIN_US = 500;
  constexpr uint16_t SERVO_MAX_US = 2400;
  constexpr uint8_t SERVO_START_ANGLE = 0;

  // Safety and network limits.
  constexpr uint16_t COMMAND_TIMEOUT_MS = 500;
  // Ignore duplicate drive-refresh HTTP requests faster than this; direction changes and
  // stop commands are still handled immediately by the server.
  constexpr uint16_t DRIVE_REPEAT_MIN_MS = 110;
  constexpr uint16_t SERVO_RATE_LIMIT_MS = 115;
  constexpr uint16_t HORN_RATE_LIMIT_MS = 300;
  constexpr uint16_t CONTROL_RATE_LIMIT_MS = 180;
  // Briefly coast before changing motor direction to reduce reversal current spikes.
  constexpr uint16_t MOTOR_DIRECTION_DEADTIME_MS = 85;
  constexpr uint16_t STATE_RATE_LIMIT_MS = 1000;
  constexpr uint32_t WATCHDOG_TIMEOUT_MS = 10000UL;
  // Local access is gated by the WPA2 AP password; there is no extra web PIN.
  // The HTTP portal must remain on the private local Wi-Fi network.

  // Software digital-drive profiles. FAST is continuously enabled at full driver input.
  // MEDIUM/SLOW are timed digital ON/OFF bursts (not LEDC or analogWrite).
  // This may feel pulsed/jerky; truly smooth speed control needs a motor driver with PWM or voltage control.
  constexpr uint16_t DRIVE_WINDOW_MS = 200;
  constexpr uint16_t MEDIUM_ON_MS = 145;
  constexpr uint16_t SLOW_ON_MS = 85;

  // Bounded sensorless AUTO demo timings; adjust only for safe bench tests.
  constexpr uint16_t AUTO_FORWARD_ONE_MS = 420;
  constexpr uint16_t AUTO_PAUSE_MS = 220;
  constexpr uint16_t AUTO_TURN_MS = 240;
  constexpr uint16_t AUTO_FORWARD_TWO_MS = 360;
}

#endif // SPARK_CAR_CONFIG_H
