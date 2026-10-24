#ifndef SPARK_CAR_CONTROL_TYPES_H
#define SPARK_CAR_CONTROL_TYPES_H

#include <Arduino.h>
#include <stdint.h>
#include "config.h"

enum class DriveCommand : uint8_t { Stop, Forward, Backward, Left, Right, Brake };
enum class DriveLevel : uint8_t { Slow, Medium, Fast };
enum class ControlMode : uint8_t { Manual, Auto, QLearning };

static inline bool elapsedMs(const uint32_t now, const uint32_t since, const uint32_t interval) {
  return static_cast<uint32_t>(now - since) >= interval;
}

#endif // SPARK_CAR_CONTROL_TYPES_H
