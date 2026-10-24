#ifndef SPARK_CAR_ACCESSORIES_H
#define SPARK_CAR_ACCESSORIES_H

#include <Arduino.h>
#include <ESP32Servo.h>
#include <stdint.h>
#include "config.h"

// Accessory wrappers: ball servo, RGB status LED, and buzzer.
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

class SparkStatusLed {
 public:
  void begin() {
    pinMode(SparkConfig::LED_RED_PIN, OUTPUT);
    pinMode(SparkConfig::LED_GREEN_PIN, OUTPUT);
    pinMode(SparkConfig::LED_BLUE_PIN, OUTPUT);
    set(LOW, LOW, LOW);
    idle();
  }
  void idle() { set(LOW, HIGH, HIGH); }
  void moving() { set(LOW, HIGH, LOW); }
  void warning() { set(HIGH, HIGH, LOW); }
  void stopped() { set(HIGH, LOW, LOW); }
 private:
  void set(const uint8_t red, const uint8_t green, const uint8_t blue) {
    digitalWrite(SparkConfig::LED_RED_PIN, red);
    digitalWrite(SparkConfig::LED_GREEN_PIN, green);
    digitalWrite(SparkConfig::LED_BLUE_PIN, blue);
  }
};

class SparkBuzzer {
 public:
  void begin() { pinMode(SparkConfig::BUZZER_PIN, OUTPUT); noTone(SparkConfig::BUZZER_PIN); }
  void beep(const uint16_t frequency, const uint16_t durationMs) {
    tone(SparkConfig::BUZZER_PIN, frequency, durationMs);
  }
};

#endif // SPARK_CAR_ACCESSORIES_H
