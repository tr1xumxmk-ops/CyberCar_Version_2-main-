# PCB Fabrication and Design Review — Rev A

## Design intent

This is a **low-current controller carrier / interconnect PCB** for the CyberCar SPARK REDLINE build. It is not a replacement for the LOLIN S2 Mini, L298N module, or 5 V buck converter. Those remain separate modules and connect through labeled harnesses.

- Board outline: 110 × 88 mm nominal inside the Edge.Cuts boundary (coordinates x=1..111, y=1..89).
- Layers: 2 copper layers, 1.6 mm board thickness.
- Suggested fabrication: FR-4, 1 oz copper, black solder mask and yellow/white silkscreen if the fab offers it.
- Mounting: four non-plated 3.2 mm holes around (4,4), (108,4), (4,86), (108,86) mm.
- Signal traces: approximately 0.35–0.40 mm. Regulated 5 V distribution uses 0.8 mm in the draft; review against actual current, copper, temperature rise and fab rules.
- Ground: bottom-copper GND zone. In KiCad, refill zones with **B** before running DRC.

## Before ordering boards

1. Open `CyberCar_SPARK_Redline_Carrier.kicad_pcb` in KiCad PCB Editor.
2. Refill zones with **B** and run **Inspect → Design Rules Checker**.
3. Resolve all unconnected pads, clearance errors, silkscreen overlaps, and unconnected zone islands. The included board has not been validated in KiCad on this build machine.
4. Confirm the terminal block and pin-header pitch/footprints match the exact parts you will buy. These are custom generic footprints, not guaranteed to match every vendor's body size.
5. Confirm Q1's actual TO-92 lead order. It varies by transistor vendor and series; do not force leads into the footprint without checking the datasheet.
6. Confirm servo lead order and current. Brown/black is often ground, red often supply, orange/yellow often signal, but check the actual servo.
7. Check that C1 polarity and 470 µF/10 V voltage rating are correct; the plus pad is pad 1.
8. Check connectors and all tracks with a multimeter before connecting a powered ESP32.
9. Generate Gerbers and drill files from KiCad only after the above checks pass. Gerbers are intentionally not provided as a fabricated-ready release yet.
10. Assemble and first power up on a current-limited 5 V bench supply if available.

## Power separation (critical)

- J1 accepts **regulated 5.00 V only** from the external buck converter. It is not a battery input.
- The 2S LiPo motor supply is wired directly to the L298N motor-supply terminals using appropriately rated wire and connectors. Motor current never flows through this carrier PCB.
- Connect L298N ground to J3 pin 1 so logic and driver share ground. Battery negative, buck negative, ESP32 GND, servo GND and L298N GND must be common.
- Do not connect carrier +5 V to the L298N module's +5 V terminal; L298N module regulator/jumper designs vary.
- Avoid feeding 5 V from this board and USB simultaneously unless the exact LOLIN board's power-path isolation has been verified.
- J5 is a low-current 3.3 V logic expansion header only. Never connect 5 V logic to the ESP32 GPIO.

## Limits

This carrier does not provide reverse-polarity protection, overcurrent protection, a battery charger/BMS, motor power switching, or safety-rated emergency stop. Use an appropriately rated physical battery disconnect and suitable external protection. This is an engineering prototype; the designer/user must do an independent design review and bench test before using it on a moving robot.
