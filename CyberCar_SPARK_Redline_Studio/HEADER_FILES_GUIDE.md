# C++ Header Files — quick guide using CyberCar NEON

A header file (`.h`) is a place to declare reusable constants, types, and class interfaces. It helps keep a growing project organized instead of putting every class in the `.ino` file.

## 1. What the files in this project do

- `config.h`: one source of truth for GPIO pin numbers, Wi-Fi settings, timing values, and enums. Hardware constants use `constexpr` so they are known at compile time.
- `motor_driver.h`: the reusable `MotorDriver` class. It accepts the six L298N pins and provides `begin()`, `apply()`, `setLevel()`, and `tick()`.
- `captive_portal.h`: web server routes and the UI. It calls the public methods of `CarController` without directly manipulating pins.
- `CyberCarPro_optimized.ino`: the application entry point. It creates objects and owns Arduino's required `setup()` and `loop()` functions.

## 2. Create a header in Arduino IDE

1. Open the sketch folder, not a random folder elsewhere.
2. In Arduino IDE 2, use the tab menu / **New Tab** and create `example_device.h`. In a file manager/editor, save the file in the same folder as the `.ino` file.
3. Start it with `#pragma once` (or a classic include guard).
4. Include the dependencies it needs, such as `Arduino.h` or `config.h`.
5. Include the header from the `.ino` using `#include "example_device.h"`.
6. Keep global function definitions from being duplicated. A simple pattern is to put small class method definitions inside the class body, or declare methods in the header and define them once in a matching `.cpp` file.
7. Keep an entire Arduino sketch and its included headers in one sketch directory. `#include "file.h"` is a local project include; angle brackets such as `#include <WiFi.h>` normally load an installed/core library.

## 3. A tiny example

Create `status_flag.h`:

```cpp
#pragma once
#include <Arduino.h>

namespace DeviceConfig {
  constexpr uint8_t STATUS_PIN = 9;
}

class StatusFlag {
 public:
  void begin() {
    pinMode(DeviceConfig::STATUS_PIN, OUTPUT);
    digitalWrite(DeviceConfig::STATUS_PIN, LOW);
  }

  void set(bool on) {
    digitalWrite(DeviceConfig::STATUS_PIN, on ? HIGH : LOW);
  }
};
```

Then use it in `main.ino`:

```cpp
#include "status_flag.h"
StatusFlag statusFlag;

void setup() {
  statusFlag.begin();
  statusFlag.set(true);
}

void loop() {
  // Do other non-blocking tasks here.
}
```

`constexpr` is right for fixed compile-time values such as pins, durations and array sizes. Do not use it for a reading that changes at runtime, such as `millis()`, a joystick position, a sensor measurement, or available heap memory.

## 4. Good habits

- Use `#pragma once` or `#ifndef ... #define ... #endif` to avoid including a header twice.
- Use namespaces to avoid global-name collisions (`Config::MOTOR_IN1_PIN`).
- Keep pin assignments in one config header rather than copying them into many files.
- Prefer fixed-size buffers and small JSON responses on embedded systems; avoid storing large images/animation frames in RAM.
- Header files should define reusable behavior, not contain a second `setup()`/`loop()`.
- If multiple `.cpp` files include the same header, avoid defining non-`inline` global variables or non-`inline` global functions in the header. Put those definitions in one `.cpp` file.

## 5. Why the web UI uses CSS animations

CSS keyframes animate the car illustration and neon grid in the phone browser. The ESP32 only serves the HTML once; it does not generate frame images or send an animation stream. That keeps network traffic and ESP32 heap usage low. The UI also honors `prefers-reduced-motion` for accessibility.

## 6. New `secure_backend.h`
`secure_backend.h` keeps reusable HTTP hardening and validation in one place: security headers, strict bounded integer parsing, fixed-value allowlists, constant-time token/PIN comparison, and JSON-string escaping. `captive_portal.h` includes it and calls those helpers. Keep it alongside the `.ino` and other local headers; do not move it into a random downloads folder. It intentionally does not parse arbitrary JSON bodies: the current API has only a few fixed commands, so validating the specific fields is simpler and reduces attack surface.
