#ifndef SPARK_CAR_WEB_ROUTES_H
#define SPARK_CAR_WEB_ROUTES_H

#include <Arduino.h>
#include <WebServer.h>
#include <esp_system.h>
#include <stdint.h>
#include <stdio.h>
#include "config.h"
#include "control_types.h"
#include "car_controller.h"
#include "secure_backend.h"
#include "captive_portal.h"

// Validated local-control API handlers.
static void routeJoystick() {
  if (!requireTrustedOrigin()) return;
  if (requestRateLimited(lastJoyRequestAt, SparkConfig::JOY_RATE_LIMIT_MS))
    return sendJson(429, "{\"ok\":false,\"error\":\"rate_limited\"}");
  int x = 0, y = 0;
  if (!parseBoundedInt("x", -100, 100, x) || !parseBoundedInt("y", -100, 100, y))
    return sendJson(400, "{\"ok\":false,\"error\":\"invalid_joystick\"}");
  if (!car.driveFromJoystick(x, y, millis())) {
    if (car.estop()) return sendJson(423, "{\"ok\":false,\"error\":\"emergency_stop_latched\"}");
    return sendJson(409, "{\"ok\":false,\"error\":\"mode_locked\"}");
  }
  sendJson(200, "{\"ok\":true}");
}

static void routeSpeed() {
  if (!requireTrustedOrigin()) return;
  if (requestRateLimited(lastSpeedRequestAt, SparkConfig::CONTROL_RATE_LIMIT_MS))
    return sendJson(429, "{\"ok\":false,\"error\":\"rate_limited\"}");
  if (!webServer.hasArg("level")) return sendJson(400, "{\"ok\":false,\"error\":\"missing_level\"}");
  const String level = webServer.arg("level");
  if (level == "fast") car.setLevel(DriveLevel::Fast);
  else if (level == "medium") car.setLevel(DriveLevel::Medium);
  else if (level == "slow") car.setLevel(DriveLevel::Slow);
  else return sendJson(400, "{\"ok\":false,\"error\":\"invalid_level\"}");
  sendJson(200, "{\"ok\":true}");
}

static void routeServo() {
  if (!requireTrustedOrigin()) return;
  if (requestRateLimited(lastServoRequestAt, SparkConfig::CONTROL_RATE_LIMIT_MS))
    return sendJson(429, "{\"ok\":false,\"error\":\"rate_limited\"}");
  int angle = 0;
  if (!parseBoundedInt("angle", 0, 180, angle))
    return sendJson(400, "{\"ok\":false,\"error\":\"invalid_angle\"}");
  if (!car.setServoAngle(angle))
    return sendJson(503, "{\"ok\":false,\"error\":\"servo_not_attached\"}");
  sendJson(200, "{\"ok\":true}");
}

static void routeHorn() {
  if (!requireTrustedOrigin()) return;
  car.horn();
  sendJson(200, "{\"ok\":true}");
}

static void routeBrake() {
  if (!requireTrustedOrigin()) return;
  car.emergencyStop(millis());
  sendJson(200, "{\"ok\":true,\"estop\":true}");
}

static void routeResume() {
  if (!requireTrustedOrigin()) return;
  car.resume(millis());
  sendJson(200, "{\"ok\":true,\"estop\":false}");
}

static void routeMode() {
  if (!requireTrustedOrigin()) return;
  if (requestRateLimited(lastModeRequestAt, SparkConfig::CONTROL_RATE_LIMIT_MS))
    return sendJson(429, "{\"ok\":false,\"error\":\"rate_limited\"}");
  if (!webServer.hasArg("mode")) return sendJson(400, "{\"ok\":false,\"error\":\"missing_mode\"}");
  const String mode = webServer.arg("mode");
  ControlMode requested;
  if (mode == "manual") requested = ControlMode::Manual;
  else if (mode == "auto") requested = ControlMode::Auto;
  else if (mode == "qlearning") requested = ControlMode::QLearning;
  else return sendJson(400, "{\"ok\":false,\"error\":\"invalid_mode\"}");
  if (!car.setMode(requested, millis()))
    return sendJson(423, "{\"ok\":false,\"error\":\"emergency_stop_latched\"}");
  if (requested == ControlMode::Auto)
    sendJson(200, "{\"ok\":true,\"mode\":\"auto\",\"note\":\"short_sensorless_demo\"}");
  else if (requested == ControlMode::QLearning)
    sendJson(200, "{\"ok\":true,\"mode\":\"qlearning\",\"note\":\"motors_held_until_feedback_sensor\"}");
  else sendJson(200, "{\"ok\":true,\"mode\":\"manual\"}");
}

static void routeState() {
  if (requestRateLimited(lastStateRequestAt, SparkConfig::STATE_RATE_LIMIT_MS))
    return sendJson(429, "{\"ok\":false,\"error\":\"rate_limited\"}");
  char response[320];
  snprintf(response, sizeof(response),
    "{\"ok\":true,\"command\":\"%s\",\"level\":\"%s\",\"mode\":\"%s\",\"estop\":%s,\"servoAngle\":%u,\"servoAttached\":%s,\"heap\":%lu,\"uptime\":%lu}",
    MotorDriver::commandName(car.motor().command()),
    MotorDriver::levelName(car.motor().level()),
    CarController::modeName(car.mode()),
    car.estop() ? "true" : "false",
    static_cast<unsigned>(car.servo().angle()),
    car.servo().attached() ? "true" : "false",
    static_cast<unsigned long>(ESP.getFreeHeap()),
    static_cast<unsigned long>(millis() / 1000UL));
  sendJson(200, response);
}

static void routeNotFound() {
  if (webServer.uri().startsWith("/api/")) return sendJson(404, "{\"ok\":false,\"error\":\"not_found\"}");
  if (webServer.uri().startsWith("/assets/")) { webServer.send(404, "text/plain; charset=utf-8", "Not found"); return; }
  routeIndex(); // captive-portal convenience for browser navigation
}

#endif // SPARK_CAR_WEB_ROUTES_H
