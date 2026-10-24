#ifndef SPARK_CAR_MOTOR_DRIVER_H
#define SPARK_CAR_MOTOR_DRIVER_H

#include <Arduino.h>
#include <stdint.h>
#include "config.h"
#include "control_types.h"

// L298N digital motor outputs. No LEDC or analogWrite motor-control calls.
class MotorDriver {
 public:
  void begin() {
    pinMode(SparkConfig::MOTOR_IN1_PIN, OUTPUT);
    pinMode(SparkConfig::MOTOR_IN2_PIN, OUTPUT);
    pinMode(SparkConfig::MOTOR_IN3_PIN, OUTPUT);
    pinMode(SparkConfig::MOTOR_IN4_PIN, OUTPUT);
    pinMode(SparkConfig::MOTOR_ENA_PIN, OUTPUT);
    pinMode(SparkConfig::MOTOR_ENB_PIN, OUTPUT);
    writeEnable(false);
    writeDirections(LOW, LOW, LOW, LOW);
    command_ = DriveCommand::Stop;
    level_ = DriveLevel::Fast;
    emergencyBrake_ = false;
    outputsReady_ = true;
    windowStartedAt_ = millis();
  }

  void setLevel(const DriveLevel level) {
    if (level_ == level) return;
    level_ = level;
    windowStartedAt_ = millis();
    tick(windowStartedAt_);
  }

  void apply(const DriveCommand command, const uint32_t now) {
    if (!outputsReady_) return;
    if (command == command_ && command != DriveCommand::Brake) return;
    writeEnable(false); // break-before-make while changing H-bridge direction
    emergencyBrake_ = (command == DriveCommand::Brake);
    switch (command) {
      case DriveCommand::Forward:  setDirection(true, false, true, false); break;
      case DriveCommand::Backward: setDirection(false, true, false, true); break;
      case DriveCommand::Left:     setDirection(false, true, true, false); break;
      case DriveCommand::Right:    setDirection(true, false, false, true); break;
      case DriveCommand::Brake:    writeDirections(HIGH, HIGH, HIGH, HIGH); break;
      case DriveCommand::Stop:     writeDirections(LOW, LOW, LOW, LOW); break;
    }
    command_ = command;
    windowStartedAt_ = now;
    tick(now);
  }

  void tick(const uint32_t now) {
    if (!outputsReady_) return;
    if (command_ == DriveCommand::Stop) {
      writeEnable(false);
      return;
    }
    if (command_ == DriveCommand::Brake || emergencyBrake_) {
      writeEnable(true);
      return;
    }
    if (level_ == DriveLevel::Fast) {
      writeEnable(true); // maximum continuous enable; no motor PWM peripheral
      return;
    }
    constexpr uint16_t kWindow = SparkConfig::DRIVE_WINDOW_MS;
    const uint16_t onMs = (level_ == DriveLevel::Medium)
        ? SparkConfig::MEDIUM_ON_MS : SparkConfig::SLOW_ON_MS;
    const uint16_t phase = static_cast<uint16_t>((now - windowStartedAt_) % kWindow);
    writeEnable(phase < onMs);
  }

  void stop(const uint32_t now) { apply(DriveCommand::Stop, now); }
  void brake(const uint32_t now) { apply(DriveCommand::Brake, now); }
  DriveCommand command() const { return command_; }
  DriveLevel level() const { return level_; }
  bool isBraking() const { return emergencyBrake_; }

  static const char* commandName(const DriveCommand value) {
    switch (value) {
      case DriveCommand::Forward: return "forward";
      case DriveCommand::Backward: return "backward";
      case DriveCommand::Left: return "left";
      case DriveCommand::Right: return "right";
      case DriveCommand::Brake: return "brake";
      default: return "stop";
    }
  }
  static const char* levelName(const DriveLevel value) {
    switch (value) {
      case DriveLevel::Slow: return "slow";
      case DriveLevel::Medium: return "medium";
      default: return "fast";
    }
  }

 private:
  void setDirection(bool a, bool b, bool c, bool d) {
    if (SparkConfig::INVERT_LEFT_MOTOR) { const bool tmp = a; a = b; b = tmp; }
    if (SparkConfig::INVERT_RIGHT_MOTOR) { const bool tmp = c; c = d; d = tmp; }
    writeDirections(a ? HIGH : LOW, b ? HIGH : LOW, c ? HIGH : LOW, d ? HIGH : LOW);
  }
  void writeDirections(const uint8_t a, const uint8_t b, const uint8_t c, const uint8_t d) {
    digitalWrite(SparkConfig::MOTOR_IN1_PIN, a);
    digitalWrite(SparkConfig::MOTOR_IN2_PIN, b);
    digitalWrite(SparkConfig::MOTOR_IN3_PIN, c);
    digitalWrite(SparkConfig::MOTOR_IN4_PIN, d);
  }
  void writeEnable(const bool enabled) {
    if (enableKnown_ && enabled == enabled_) return;
    digitalWrite(SparkConfig::MOTOR_ENA_PIN, enabled ? HIGH : LOW);
    digitalWrite(SparkConfig::MOTOR_ENB_PIN, enabled ? HIGH : LOW);
    enableKnown_ = true;
    enabled_ = enabled;
  }

  DriveCommand command_ = DriveCommand::Stop;
  DriveLevel level_ = DriveLevel::Fast;
  uint32_t windowStartedAt_ = 0;
  bool outputsReady_ = false;
  bool emergencyBrake_ = false;
  bool enableKnown_ = false;
  bool enabled_ = false;
};

#endif // SPARK_CAR_MOTOR_DRIVER_H
