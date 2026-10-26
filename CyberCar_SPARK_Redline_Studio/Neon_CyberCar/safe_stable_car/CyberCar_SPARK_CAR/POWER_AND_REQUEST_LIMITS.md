# SPARK CAR — Power, buzzer, and request-rate notes

## Recommended power wiring

**Yes, I recommend a separate 5 V buck converter.** In this car, the ESP32-S2 Wi-Fi radio and the servo can create short current peaks. The 5 V regulator fitted to many L298N modules is primarily there to provide the module's logic supply; its usable spare current varies by board, motor supply voltage, regulator, and cooling. Do not assume it can reliably power the ESP32 and servo together.

A sensible starting point is a quality **5 V, 3 A buck converter** (or higher if the servo/other 5 V loads require it). Check the actual servo stall current and converter temperature. The battery itself must still be adequately charged and able to supply the motors plus the converter; a buck converter cannot compensate for an exhausted battery. Wire it as follows:

1. Battery positive goes to the L298N motor-supply input and to the buck converter input positive.
2. Battery negative goes to L298N GND and buck input negative.
3. Adjust the buck output to **5.0 V with a multimeter before connecting electronics**.
4. Buck 5 V output powers the servo's V+ and the ESP32-S2 Mini board's documented 5 V/VBUS input. **Never feed 5 V into the 3V3 pin.**
5. Connect ESP32 GND, servo GND, buck GND, and L298N GND together. Keep motor current paths away from the ESP32's thin ground/power wires where possible.
6. **Do not connect buck OUT+ to the L298N's 5 V terminal while its onboard 5 V regulator jumper is fitted.** That can tie two power sources together. Keep the buck powering the ESP32/servo and leave the L298N logic supply configured according to the exact module's jumper instructions.
7. Do not connect USB and an external 5 V source simultaneously unless you have verified that your exact board protects against back-feeding. Disconnect the external 5 V feed when programming over USB, if unsure.

If the battery is a single 3.7 V cell, this wiring needs a boost converter instead of a buck; the converter type depends on battery voltage.

## Buzzer caveat

The sketch drives the configured buzzer signal GPIO, `BUZZER_PIN` (currently GPIO 18). It does not create a second physical buzzer. **A standard L298N motor-driver module normally has no built-in buzzer**, so software alone cannot make an L298N beep. If your particular all-in-one driver board has an onboard buzzer, its control/signal pin must be identified and `BUZZER_PIN` must match the GPIO connected to that signal. Do not guess the pin because it may conflict with a motor or servo pin. The current value GPIO 18 is correct only if your existing buzzer signal is actually connected there.

## Physical LED

The physical RGB LED GPIO setup and code have been removed. The small status dot visible on the webpage is only a browser UI indicator, not a physical LED output.

## Request pacing and webpage behavior

- Drive hold refresh is sent every 220 ms, below the firmware's 500 ms drive safety timeout.
- Repeated identical drive requests are coalesced; genuine direction changes and stop commands are handled immediately.
- Servo commands are paced to at most about 8 per second and the latest slider position wins.
- Status polling is reduced to once every 1.5 seconds.
- Horn and mode/speed endpoints have server-side cooldowns.
- Requests time out in the browser instead of hanging forever; on a drive request failure the browser releases the control and the firmware's safety timeout stops the motors.
- The page contains no automatic `location.reload()` behavior. Some phones automatically close their captive-portal mini-browser or switch away from a Wi-Fi network with no internet; that OS behavior cannot be prevented by ESP32 webpage JavaScript. Open `http://192.168.4.1/` in a normal browser and choose the phone's “stay connected/use this network without internet” option if offered.

Test with wheels lifted and keep a physical battery disconnect accessible.
