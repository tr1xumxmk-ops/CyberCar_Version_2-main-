// Reusable digital-only L298N motor driver.
#pragma once
#include <Arduino.h>
#include "config.h"

class MotorDriver {
 public:
  constexpr MotorDriver(const uint8_t in1, const uint8_t in2,
                        const uint8_t in3, const uint8_t in4,
                        const uint8_t ena, const uint8_t enb)
      : in1_(in1), in2_(in2), in3_(in3), in4_(in4), ena_(ena), enb_(enb) {}

  void begin() {
    pinMode(in1_, OUTPUT); pinMode(in2_, OUTPUT);
    pinMode(in3_, OUTPUT); pinMode(in4_, OUTPUT);
    pinMode(ena_, OUTPUT); pinMode(enb_, OUTPUT);
    writeEnable(false);
    writeDirections(LOW, LOW, LOW, LOW);
    activeCommand_ = DriveCommand::Stop;
    outputsReady_ = true;
    emergencyBrake_ = false;
    windowStartedAt_ = millis();
  }

  void setLevel(const DriveLevel level) {
    if (level_ == level) return;
    level_ = level;
    windowStartedAt_ = millis();
    if (activeCommand_ != DriveCommand::Stop && activeCommand_ != DriveCommand::Brake) {
      tick(windowStartedAt_);
    }
  }

  // Calling the same direction refreshes no timer, so joystick keepalives do not reset the burst cycle.
  void apply(const DriveCommand command, const uint32_t now) {
    if (!outputsReady_) return;
    if (command == activeCommand_ && (command != DriveCommand::Brake || emergencyBrake_)) return;

    // Disable first, change H-bridge direction second, then enable.
    writeEnable(false);
    emergencyBrake_ = (command == DriveCommand::Brake);

    switch (command) {
      case DriveCommand::Forward:  setDirection(true, false, true, false); break;
      case DriveCommand::Backward: setDirection(false, true, false, true); break;
      case DriveCommand::Left:     setDirection(false, true, true, false); break;
      case DriveCommand::Right:    setDirection(true, false, false, true); break;
      case DriveCommand::Brake:    writeDirections(HIGH, HIGH, HIGH, HIGH); break;
      case DriveCommand::Stop:     writeDirections(LOW, LOW, LOW, LOW); break;
    }

    activeCommand_ = command;
    windowStartedAt_ = now;
    tick(now);
  }

  void tick(const uint32_t now) {
    if (!outputsReady_) return;
    if (activeCommand_ == DriveCommand::Stop) {
      writeEnable(false);
      return;
    }
    if (activeCommand_ == DriveCommand::Brake || emergencyBrake_) {
      writeEnable(true);
      return;
    }

    if (level_ == DriveLevel::Fast) {
      writeEnable(true);  // continuously enabled, full supply voltage
      return;
    }

    constexpr uint16_t kWindow = Config::DRIVE_WINDOW_MS;
    const uint16_t onMs = (level_ == DriveLevel::Medium)
        ? Config::MEDIUM_ON_MS : Config::SLOW_ON_MS;
    const uint16_t phase = static_cast<uint16_t>((now - windowStartedAt_) % kWindow);
    writeEnable(phase < onMs);
  }

  void stop(const uint32_t now) { apply(DriveCommand::Stop, now); }
  void brake(const uint32_t now) { apply(DriveCommand::Brake, now); }
  DriveCommand activeCommand() const { return activeCommand_; }
  DriveLevel level() const { return level_; }
  bool isBraking() const { return emergencyBrake_; }

  static const char* commandName(const DriveCommand command) {
    switch (command) {
      case DriveCommand::Forward: return "forward";
      case DriveCommand::Backward: return "backward";
      case DriveCommand::Left: return "left";
      case DriveCommand::Right: return "right";
      case DriveCommand::Brake: return "brake";
      default: return "stop";
    }
  }
  static const char* levelName(const DriveLevel level) {
    switch (level) {
      case DriveLevel::Slow: return "slow";
      case DriveLevel::Medium: return "medium";
      default: return "fast";
    }
  }

 private:
  void setDirection(bool a, bool b, bool c, bool d) {
    if (Config::INVERT_LEFT_MOTOR) { const bool t=a; a=b; b=t; }
    if (Config::INVERT_RIGHT_MOTOR) { const bool t=c; c=d; d=t; }
    writeDirections(a ? HIGH : LOW, b ? HIGH : LOW, c ? HIGH : LOW, d ? HIGH : LOW);
  }
  void writeDirections(const uint8_t a, const uint8_t b, const uint8_t c, const uint8_t d) {
    digitalWrite(in1_, a); digitalWrite(in2_, b);
    digitalWrite(in3_, c); digitalWrite(in4_, d);
  }
  void writeEnable(const bool enabled) {
    if (enabledStateKnown_ && enabled == enabledState_) return;
    digitalWrite(ena_, enabled ? HIGH : LOW);
    digitalWrite(enb_, enabled ? HIGH : LOW);
    enabledState_ = enabled;
    enabledStateKnown_ = true;
  }

  const uint8_t in1_, in2_, in3_, in4_, ena_, enb_;
  DriveLevel level_ = DriveLevel::Medium;
  DriveCommand activeCommand_ = DriveCommand::Stop;
  uint32_t windowStartedAt_ = 0;
  bool outputsReady_ = false;
  bool emergencyBrake_ = false;
  bool enabledStateKnown_ = false;
  bool enabledState_ = false;
};
