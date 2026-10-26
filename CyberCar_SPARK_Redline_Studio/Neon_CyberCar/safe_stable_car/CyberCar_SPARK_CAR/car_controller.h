#ifndef SPARK_CAR_CAR_CONTROLLER_H
#define SPARK_CAR_CAR_CONTROLLER_H

#include <Arduino.h>
#include <stdint.h>
#include "config.h"
#include "control_types.h"
#include "motor_driver.h"
#include "accessories.h"

// Coordinates drive commands, manual watchdog, AUTO demonstration and safety stop.
class CarController {
 public:
  void begin() {
    motor_.begin();
    servo_.begin();
    buzzer_.begin();
    motor_.setLevel(DriveLevel::Fast);
    motor_.stop(millis());
    lastDriveCommandAt_ = millis();
    autoStepStartedAt_ = lastDriveCommandAt_;
    emergencyLatched_ = false;
    autoRunning_ = false;
    mode_ = ControlMode::Manual;
  }

  void update(const uint32_t now) {
    if (mode_ == ControlMode::Auto && autoRunning_) updateAutoDemo(now);

    // Manual command watchdog only. AUTO has its own short, bounded sequence timer.
    if (mode_ == ControlMode::Manual && !emergencyLatched_ && isMoving(motor_.command()) &&
        elapsedMs(now, lastDriveCommandAt_, SparkConfig::COMMAND_TIMEOUT_MS)) {
      motor_.stop(now);
    }
    motor_.tick(now);
  }

  // Direct button command. The web UI sends one named direction, not joystick axes.
  // STOP also exits AUTO/QLearning and returns to MANUAL without restarting movement.
  bool driveFromButton(const DriveCommand requested, const uint32_t now) {
    if (emergencyLatched_) return false;
    if (mode_ != ControlMode::Manual) {
      if (requested == DriveCommand::Stop) {
        autoRunning_ = false;
        mode_ = ControlMode::Manual;
        motor_.stop(now);
        lastDriveCommandAt_ = now;
        return true;
      }
      return false;
    }
    motor_.apply(requested, now);
    lastDriveCommandAt_ = now;
    return true;
  }

  bool setMode(const ControlMode requested, const uint32_t now) {
    if (emergencyLatched_) return false;
    autoRunning_ = false;
    mode_ = requested;
    motor_.stop(now);
    lastDriveCommandAt_ = now;
    if (requested == ControlMode::Auto) {
      // Sensorless short demo sequence: slow digital drive, bounded duration, then stop.
      // This is not obstacle avoidance. Test with wheels raised and in a clear area.
      motor_.setLevel(DriveLevel::Slow);
      autoStep_ = 0;
      autoStepStartedAt_ = now;
      autoRunning_ = true;
      motor_.apply(DriveCommand::Forward, now);
    } else if (requested == ControlMode::QLearning) {
      // No distance/feedback sensor is installed. Hold real motors stopped; UI offers a policy simulation.
      motor_.stop(now);
    }
    buzzer_.beep(760, 35);
    return true;
  }

  void setLevel(const DriveLevel level) {
    motor_.setLevel(level);
    buzzer_.beep(850, 30);
  }
  void emergencyStop(const uint32_t now) {
    emergencyLatched_ = true;
    autoRunning_ = false;
    motor_.brake(now);
    buzzer_.beep(260, 220);
  }
  void resume(const uint32_t now) {
    emergencyLatched_ = false;
    autoRunning_ = false;
    mode_ = ControlMode::Manual;
    motor_.stop(now); // Resume unlocks but never restarts movement automatically.
    lastDriveCommandAt_ = now;
    buzzer_.beep(1100, 65);
  }
  void horn() { buzzer_.beep(950, 180); }
  bool setServoAngle(const int angle) { return servo_.setAngle(angle); }
  MotorDriver& motor() { return motor_; }
  const BallServoController& servo() const { return servo_; }
  bool estop() const { return emergencyLatched_; }
  ControlMode mode() const { return mode_; }

  static const char* modeName(const ControlMode value) {
    switch (value) {
      case ControlMode::Auto: return "auto";
      case ControlMode::QLearning: return "qlearning";
      default: return "manual";
    }
  }

 private:
  static bool isMoving(const DriveCommand value) {
    return value == DriveCommand::Forward || value == DriveCommand::Backward ||
           value == DriveCommand::Left || value == DriveCommand::Right;
  }

  void updateAutoDemo(const uint32_t now) {
    uint16_t duration = 0;
    switch (autoStep_) {
      case 0: duration = SparkConfig::AUTO_FORWARD_ONE_MS; break;
      case 1: duration = SparkConfig::AUTO_PAUSE_MS; break;
      case 2: duration = SparkConfig::AUTO_TURN_MS; break;
      case 3: duration = SparkConfig::AUTO_PAUSE_MS; break;
      case 4: duration = SparkConfig::AUTO_FORWARD_TWO_MS; break;
      default:
        motor_.stop(now);
        autoRunning_ = false;
        return;
    }
    if (!elapsedMs(now, autoStepStartedAt_, duration)) return;
    ++autoStep_;
    autoStepStartedAt_ = now;
    switch (autoStep_) {
      case 1: motor_.apply(DriveCommand::Stop, now); break;
      case 2: motor_.apply(DriveCommand::Right, now); break;
      case 3: motor_.apply(DriveCommand::Stop, now); break;
      case 4: motor_.apply(DriveCommand::Forward, now); break;
      default:
        motor_.stop(now);
        autoRunning_ = false;
        break;
    }
  }

  MotorDriver motor_;
  BallServoController servo_;
  SparkBuzzer buzzer_;
  uint32_t lastDriveCommandAt_ = 0;
  uint32_t autoStepStartedAt_ = 0;
  uint8_t autoStep_ = 0;
  ControlMode mode_ = ControlMode::Manual;
  bool autoRunning_ = false;
  bool emergencyLatched_ = false;
};

extern CarController car;

#endif // SPARK_CAR_CAR_CONTROLLER_H
