#ifndef SPARK_CAR_MOTOR_DRIVER_H
#define SPARK_CAR_MOTOR_DRIVER_H

#include <Arduino.h>
#include <stdint.h>
#include "config.h"
#include "control_types.h"

// L298N digital motor outputs. Direction changes use a non-blocking coast interval
// to reduce current spikes when the car is commanded to reverse/turn quickly.
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
    pendingCommand_ = DriveCommand::Stop;
    level_ = DriveLevel::Fast;
    emergencyBrake_ = false;
    reversalPending_ = false;
    haveLastDirection_ = false;
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

    if (command == DriveCommand::Stop) {
      if (command_ == DriveCommand::Stop && !reversalPending_) return;
      reversalPending_ = false;
      emergencyBrake_ = false;
      command_ = DriveCommand::Stop;
      pendingCommand_ = DriveCommand::Stop;
      writeEnable(false);
      writeDirections(LOW, LOW, LOW, LOW);
      windowStartedAt_ = now;
      return;
    }

    if (command == DriveCommand::Brake) {
      reversalPending_ = false;
      pendingCommand_ = DriveCommand::Brake;
      emergencyBrake_ = true;
      command_ = DriveCommand::Brake;
      writeEnable(false);
      writeDirections(HIGH, HIGH, HIGH, HIGH);
      writeEnable(true);
      return;
    }

    // The same already-active command needs no GPIO writes. Repeated refreshes of
    // a command waiting for the dead-time also must not restart that timer.
    if (!reversalPending_ && command == command_) return;
    if (reversalPending_ && command == pendingCommand_) return;

    emergencyBrake_ = false;
    const bool directionChanged = haveLastDirection_ && command != lastDirection_;
    if (reversalPending_ || directionChanged) {
      // Disable the bridge and coast before changing direction inputs. This is
      // intentionally non-blocking so Wi-Fi and the watchdog can continue running.
      writeEnable(false);
      writeDirections(LOW, LOW, LOW, LOW);
      pendingCommand_ = command;
      command_ = command; // reports the latest requested direction to the UI
      reversalPending_ = true;
      transitionStartedAt_ = now;
      return;
    }

    setDirectionForCommand(command);
    command_ = command;
    pendingCommand_ = command;
    lastDirection_ = command;
    haveLastDirection_ = true;
    windowStartedAt_ = now;
    tick(now);
  }

  void tick(const uint32_t now) {
    if (!outputsReady_) return;

    if (reversalPending_) {
      writeEnable(false);
      if (!elapsedMs(now, transitionStartedAt_, SparkConfig::MOTOR_DIRECTION_DEADTIME_MS)) return;
      const DriveCommand target = pendingCommand_;
      reversalPending_ = false;
      setDirectionForCommand(target);
      command_ = target;
      lastDirection_ = target;
      haveLastDirection_ = true;
      windowStartedAt_ = now;
      // Continue below to enable the bridge now that the dead-time has elapsed.
    }

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
  void setDirectionForCommand(const DriveCommand command) {
    switch (command) {
      // Compensate for the original observed mapping reported by the user.
      case DriveCommand::Forward:  setDirection(true, false, false, true); break;
      case DriveCommand::Backward: setDirection(false, true, true, false); break;
      case DriveCommand::Left:     setDirection(true, false, true, false); break;
      case DriveCommand::Right:    setDirection(false, true, false, true); break;
      case DriveCommand::Brake:    writeDirections(HIGH, HIGH, HIGH, HIGH); break;
      case DriveCommand::Stop:     writeDirections(LOW, LOW, LOW, LOW); break;
    }
  }

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
  DriveCommand pendingCommand_ = DriveCommand::Stop;
  DriveCommand lastDirection_ = DriveCommand::Stop;
  DriveLevel level_ = DriveLevel::Fast;
  uint32_t windowStartedAt_ = 0;
  uint32_t transitionStartedAt_ = 0;
  bool outputsReady_ = false;
  bool emergencyBrake_ = false;
  bool reversalPending_ = false;
  bool haveLastDirection_ = false;
  bool enableKnown_ = false;
  bool enabled_ = false;
};

#endif // SPARK_CAR_MOTOR_DRIVER_H
