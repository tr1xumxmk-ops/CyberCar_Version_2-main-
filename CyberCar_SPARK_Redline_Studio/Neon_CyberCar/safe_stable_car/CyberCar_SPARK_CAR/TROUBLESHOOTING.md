# SPARK CAR — ESP32-S2 Mini stability and button-control update

## What changed

- Replaced the on-screen joystick with a four-direction button pad. Hold a direction to move; release to stop. Space also stops, and WASD/arrow keys use the same direct commands.
- Added a named `/api/drive` endpoint so the browser sends `forward`, `backward`, `left`, `right`, or `stop` instead of joystick x/y values.
- Compensated for the exact direction mapping reported for this car. The previous outputs were reported as: Forward -> physical left, Backward -> physical right, Right -> physical forward, Left -> physical backward.
- Capped the ESP32 Wi-Fi maximum transmit power at 18 dBm after the AP starts.
- Removed the explicit `WiFi.setSleep(false)` setting so the firmware no longer forcibly disables Wi-Fi sleep behavior.
- Added reset-reason and free-heap messages to Serial Monitor.
- Reduced the dashboard state polling frequency and added build options to shorten HTTP line/header parsing waits on Arduino-ESP32 versions that support WebServer hardening.

## Install

1. Extract this ZIP.
2. Open `CyberCar_SPARK_CAR.ino` from the extracted `test_sake_4` folder. Keep every `.h` file and `build_opt.h` in that same sketch folder.
3. Install the **ESP32Servo** library if Arduino IDE says it is missing.
4. Select your ESP32-S2 Mini-compatible board and the correct serial port. Compile, then upload.
5. Open Serial Monitor at **115200 baud**. Join Wi-Fi `Spark-Car` with password `12345678`, then open `http://192.168.4.1/`.
6. For the first motion test, raise the drive wheels clear of the desk and keep a physical battery disconnect close by.

## Reset troubleshooting

At boot, check `[BOOT] Reset reason`:

- `brownout/low-supply-voltage`: likely supply droop when a motor or servo starts. Use a suitable regulated supply for the servo, a separate appropriate motor supply for the L298N, and connect grounds together. Do not power motors or a servo from the ESP32 3.3 V pin. Check battery condition, wiring, and connectors.
- `task-watchdog`, `interrupt-watchdog`, or `watchdog`: inspect Serial output and confirm the HTTP hardening options were picked up by the Arduino build. Update to a current Arduino-ESP32 core if the WebServer parser does not support those options.
- `panic/exception`: capture the complete Serial Monitor log/backtrace.
- Other reset reasons: check the corresponding board reset/power wiring.

The 18 dBm cap is a conservative setting, not a guaranteed cure for a clone-board fault. It reduces maximum RF output; it cannot compensate for a weak battery, a stalled servo, undersized regulator, or poor ground wiring. A brownout reset reason means the power path needs physical attention.

## Motor direction note

The motor output patterns in `motor_driver.h` now compensate for the precise mapping reported in the request. If any physical direction is still wrong, stop testing and verify L298N wiring and which motor is connected to channel A versus B. Different physical wiring can require a different mapping; no software change can infer wiring that differs from the reported behavior.

## Hardware power recommendations

- Power the L298N motor input from a suitable motor-rated supply, not the ESP32 3.3 V rail.
- Power an MG90S/SG90 servo from a regulated supply sized for its peak current; do not use the ESP32 3.3 V pin.
- Join ESP32 GND, servo-supply GND, and L298N GND.
- Keep motor current out of thin ESP32 power/ground wiring. Add suitable bulk capacitance near the motor driver/servo supply if appropriate for your design, and check for overheated regulators or loose connections.
- Remove L298N ENA/ENB jumper caps only if GPIO 11/12 are actually wired to ENA/ENB; verify the board pinout before power-up.

This project was statically inspected and updated, but it was not compiled/flashed against your exact Arduino core or physically tested on your car in this environment.


## Direction-switch and servo responsiveness updates (October 2026)

- Drive requests are no longer rejected by a short server-side rate limit. The webpage serializes requests and keeps only the newest pending command.
- Pointer-up/cancel events are associated with the active button/pointer so a previous direction button cannot cancel a newly selected direction.
- Motor outputs coast for 85 ms before changing direction. This is a non-blocking interval; it does not pause the main loop. It reduces abrupt direction-change current spikes but cannot repair weak batteries or wiring.
- Servo slider commands are coalesced so only the latest pending angle is sent, and state polling no longer resets the slider while the user is dragging it.
- A standard hobby servo's physical travel speed is determined by the servo's own motor/gearing and its supply voltage. The firmware sends the requested angle directly; it cannot make a slow servo physically exceed its rated speed. Use a regulated external servo supply within the servo's voltage/current rating and avoid mechanical binding.

If the page still disconnects during direction changes, capture the complete Serial Monitor output at 115200 baud, especially the next [BOOT] reset reason. A brownout indicates a power/wiring issue rather than a webpage-only bug.


## Power and request pacing update

See `POWER_AND_REQUEST_LIMITS.md` for the recommended 5 V buck-converter wiring, the onboard buzzer pin caveat, removal of physical RGB LED outputs, and request pacing details.
