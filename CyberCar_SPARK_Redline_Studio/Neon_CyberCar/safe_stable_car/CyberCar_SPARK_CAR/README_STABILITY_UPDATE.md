# SPARK CAR — servo responsiveness and direction-switch stability update

## Changes in this build

- Removed the unused physical RGB LED GPIO setup and all hardware LED writes. The dashboard's small online dot is only a visual webpage indicator.
- Reduced live status polling to 1.5 seconds and paced servo commands to about 8 requests/second; drive keep-alives are 220 ms and duplicate drive requests are coalesced.
- Added browser request timeouts, a horn cooldown, and server-side limits for repeated drive/servo/status requests. No automatic page reload was added.
- Added wiring notes for a separate 5 V buck converter and explained the onboard-buzzer pin caveat.

- Corrected the browser pointer lifecycle so an old direction button's `pointerup`/`lostpointercapture` event cannot cancel a newer direction command.
- Drive commands are serialized and keep the latest desired direction instead of being rejected by a short API rate limit.
- Direction changes now have an 85 ms non-blocking coast interval before the H-bridge direction inputs are changed and re-enabled.
- Servo angle requests are coalesced, the newest slider target wins, and status polling no longer moves the slider while the user is dragging it.
- Added serial logs for accepted drive commands and servo targets.

## Install

1. Extract this archive.
2. Open `CyberCar_SPARK_CAR.ino` from this folder in Arduino IDE.
3. Keep every `.h` file and `build_opt.h` beside the `.ino` file.
4. Install the `ESP32Servo` library if required and select the correct ESP32-S2 board/port.
5. Compile and upload.
6. Use Serial Monitor at 115200 baud.

## Servo speed note

The firmware sends a target angle as soon as the queued request is processed; it does not add a movement ramp. The physical speed of a conventional hobby servo is mostly determined by its motor/gearing and rated supply voltage, not by Arduino's `write(angle)` command. If the arm still moves slowly, confirm the servo model and provide a regulated external supply within its voltage/current rating. Do not power a servo from the ESP32's 3.3 V pin. Keep grounds common, and confirm that the linkage is not binding or hitting a mechanical stop.

## If the webpage disconnects/reopens

If the page disconnects during direction switching, it may mean the ESP32 reset. The non-blocking 85 ms coast interval reduces abrupt reversal-current spikes, but it cannot compensate for inadequate batteries, a weak regulator, thin/long power wiring, or a stalled motor. Raise the wheels for testing. Connect Serial Monitor before testing and record the `[BOOT] Reset reason` line after a reset. `brownout/low-supply-voltage` points to a power problem; watchdog/panic errors point to a software fault that needs its full log to diagnose.

This code was syntax-checked for the embedded JavaScript and the motor driver header was compiled with a small host-side Arduino API stub. It has not been built with the actual Arduino-ESP32 toolchain or tested on physical hardware in this environment.

See `POWER_AND_REQUEST_LIMITS.md` for detailed power wiring, request pacing, and buzzer configuration notes.
