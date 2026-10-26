#ifndef SPARK_CAR_ACCESSORIES_H
#define SPARK_CAR_ACCESSORIES_H

#include <Arduino.h>
#include <ESP32Servo.h>
#include <stdint.h>
#include "config.h"

// Accessory wrappers: ball servo and the configured existing buzzer output.
class BallServoController {
 public:
  void begin() {
    servo_.setPeriodHertz(50);
    attached_ = servo_.attach(SparkConfig::BALL_SERVO_PIN,
                              SparkConfig::SERVO_MIN_US,
                              SparkConfig::SERVO_MAX_US) >= 0;
    angle_ = SparkConfig::SERVO_START_ANGLE;
    if (attached_) servo_.write(angle_); // requested starting position: 0 degrees
  }
  bool setAngle(const int angle) {
    if (angle < 0 || angle > 180 || !attached_) return false;
    angle_ = static_cast<uint8_t>(angle);
    servo_.write(angle_);
    return true;
  }
  uint8_t angle() const { return angle_; }
  bool attached() const { return attached_; }
 private:
  Servo servo_;
  uint8_t angle_ = SparkConfig::SERVO_START_ANGLE;
  bool attached_ = false;
};

class SparkBuzzer {
 public:
  void begin() {
    pinMode(SparkConfig::BUZZER_PIN, OUTPUT);
    noTone(SparkConfig::BUZZER_PIN);
    digitalWrite(SparkConfig::BUZZER_PIN, LOW);
  }

  void beep(const uint16_t frequency, const uint16_t durationMs) {
    // Drives the existing buzzer signal connected to BUZZER_PIN; it does not create
    // or require a second buzzer. The driver-board buzzer must expose a controllable
    // signal pin wired to this GPIO.
    tone(SparkConfig::BUZZER_PIN, frequency, durationMs);
  }
};

#endif // SPARK_CAR_ACCESSORIES_H
