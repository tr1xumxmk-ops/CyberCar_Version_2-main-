// CYBER CAR NEON — lightweight firmware entry point
#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <esp_task_wdt.h>
#include <esp_system.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "motor_driver.h"

class StatusLED {
 public:
  constexpr StatusLED(const uint8_t red, const uint8_t green, const uint8_t blue)
      : red_(red), green_(green), blue_(blue) {}
  void begin() {
    pinMode(red_, OUTPUT); pinMode(green_, OUTPUT); pinMode(blue_, OUTPUT);
    set(LOW, LOW, LOW);
  }
  void idle() { set(LOW, HIGH, HIGH); }
  void moving() { set(LOW, HIGH, LOW); }
  void warning() { set(HIGH, HIGH, LOW); }
  void stopped() { set(HIGH, LOW, LOW); }
 private:
  void set(const uint8_t r, const uint8_t g, const uint8_t b) {
    digitalWrite(red_, r); digitalWrite(green_, g); digitalWrite(blue_, b);
  }
  const uint8_t red_, green_, blue_;
};

class SoundManager {
 public:
  constexpr explicit SoundManager(const uint8_t pin) : pin_(pin) {}
  void begin() { pinMode(pin_, OUTPUT); noTone(pin_); }
  void beep(const uint16_t frequency, const uint16_t durationMs) {
    tone(pin_, frequency, durationMs);
    stopAt_ = millis() + durationMs;
    active_ = true;
  }
  void update(const uint32_t now) {
    if (active_ && static_cast<int32_t>(now - stopAt_) >= 0) {
      noTone(pin_);
      active_ = false;
    }
  }
 private:
  const uint8_t pin_;
  uint32_t stopAt_ = 0;
  bool active_ = false;
};

class BallServo {
 public:
  constexpr explicit BallServo(const uint8_t pin) : pin_(pin) {}
  void begin() {
    servo_.setPeriodHertz(50);
    attached_ = servo_.attach(pin_, Config::SERVO_MIN_US, Config::SERVO_MAX_US) >= 0;
    lock();
  }
  void lock() {
    if (attached_) servo_.write(Config::BALL_LOCKED_ANGLE);
    locked_ = true;
  }
  void release() {
    if (attached_) servo_.write(Config::BALL_RELEASED_ANGLE);
    locked_ = false;
  }
  bool locked() const { return locked_; }
  bool attached() const { return attached_; }
 private:
  const uint8_t pin_;
  Servo servo_;
  bool attached_ = false;
  bool locked_ = true;
};

class CarController {
 public:
  CarController(MotorDriver& motor, StatusLED& led, SoundManager& sound, BallServo& ball)
      : motor_(motor), led_(led), sound_(sound), ball_(ball) {}

  void begin(const uint32_t now) {
    motor_.begin();
    led_.begin();
    sound_.begin();
    ball_.begin();
    motor_.setLevel(DriveLevel::Medium);
    motor_.stop(now);
    lastCommandAt_ = now;
    emergencyLatched_ = false;
    led_.idle();
  }

  void update(const uint32_t now) {
    sound_.update(now);
    if (emergencyLatched_) {
      motor_.tick(now);
      return;
    }
    const DriveCommand cmd = motor_.activeCommand();
    if (isMoving(cmd) && elapsed(now, lastCommandAt_, Config::MANUAL_COMMAND_TIMEOUT_MS)) {
      motor_.stop(now);
      led_.warning();
    }
    motor_.tick(now);
  }

  bool handleJoystick(const char key, const uint32_t now) {
    if (emergencyLatched_) return key == 'S';  // a joystick release never clears the emergency latch
    DriveCommand command;
    switch (key) {
      case 'F': command = DriveCommand::Forward; break;
      case 'B': command = DriveCommand::Backward; break;
      case 'L': command = DriveCommand::Left; break;
      case 'R': command = DriveCommand::Right; break;
      case 'S': command = DriveCommand::Stop; break;
      default: return false;
    }
    motor_.apply(command, now);
    lastCommandAt_ = now;
    if (command == DriveCommand::Stop) led_.idle();
    else led_.moving();
    return true;
  }

  bool setDriveLevel(const DriveLevel level) {
    motor_.setLevel(level);
    sound_.beep(850, 35);
    return true;
  }

  void emergencyStop(const uint32_t now) {
    emergencyLatched_ = true;
    motor_.brake(now);
    led_.stopped();
    sound_.beep(260, 250);
  }

  void resume(const uint32_t now) {
    emergencyLatched_ = false;
    motor_.stop(now);
    lastCommandAt_ = now;
    led_.idle();
    sound_.beep(1100, 70);
  }

  void horn() { sound_.beep(950, 180); }
  void lockBall() { ball_.lock(); sound_.beep(700, 45); }
  void releaseBall() { ball_.release(); sound_.beep(1250, 75); }

  bool emergencyLatched() const { return emergencyLatched_; }
  MotorDriver& motor() { return motor_; }
  const BallServo& ball() const { return ball_; }
  uint32_t uptimeSeconds() const { return millis() / 1000U; }

 private:
  static bool isMoving(const DriveCommand cmd) {
    return cmd == DriveCommand::Forward || cmd == DriveCommand::Backward ||
           cmd == DriveCommand::Left || cmd == DriveCommand::Right;
  }
  MotorDriver& motor_;
  StatusLED& led_;
  SoundManager& sound_;
  BallServo& ball_;
  uint32_t lastCommandAt_ = 0;
  bool emergencyLatched_ = false;
};

MotorDriver motors(Config::MOTOR_IN1_PIN, Config::MOTOR_IN2_PIN,
                   Config::MOTOR_IN3_PIN, Config::MOTOR_IN4_PIN,
                   Config::MOTOR_ENA_PIN, Config::MOTOR_ENB_PIN);
StatusLED statusLed(Config::LED_RED_PIN, Config::LED_GREEN_PIN, Config::LED_BLUE_PIN);
SoundManager soundManager(Config::BUZZER_PIN);
BallServo ballServo(Config::BALL_SERVO_PIN);
CarController car(motors, statusLed, soundManager, ballServo);
#include "captive_portal.h"
PortalServer portal(car);

void setup() {
  Serial.begin(115200);
  WiFi.persistent(false);
  WiFi.setSleep(false);  // lower joystick latency while car is controlled

  const esp_task_wdt_config_t watchdogConfig = {
      .timeout_ms = Config::WATCHDOG_TIMEOUT_MS,
      .idle_core_mask = (1U << portNUM_PROCESSORS) - 1U,
      .trigger_panic = true
  };
  const esp_err_t wdtResult = esp_task_wdt_init(&watchdogConfig);
  if (wdtResult == ESP_ERR_INVALID_STATE) {
    esp_task_wdt_reconfigure(&watchdogConfig);
  } else if (wdtResult != ESP_OK) {
    Serial.printf("[WARN] Watchdog init failed: %d\n", static_cast<int>(wdtResult));
  }
  const esp_err_t addResult = esp_task_wdt_add(nullptr);
  if (addResult != ESP_OK && addResult != ESP_ERR_INVALID_STATE) {
    Serial.printf("[WARN] Watchdog task registration failed: %d\n", static_cast<int>(addResult));
  }

  car.begin(millis());
  portal.begin();
}

void loop() {
  esp_task_wdt_reset();
  portal.update();
  // Refresh time AFTER handling HTTP: command timestamps must not be compared to stale millis().
  car.update(millis());
  delay(4);
}
