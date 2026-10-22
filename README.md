[SECURE_BACKEND.md](https://github.com/user-attachments/files/33843694/SECURE_BACKEND.md)
<p align="center">
  <img src="docs/assets/readme/portal-dashboard.png" alt="CyberCar SPARK REDLINE dashboard preview" width="900">
</p>

<h1 align="center">CYBERCAR SPARK REDLINE</h1>
<p align="center">
  <strong>BLACK · HAZARD YELLOW · SIGNAL RED</strong><br>
  <em>A modular, offline-first Wi-Fi robot car and carrier-PCB engineering project by SPARK Team.</em>
</p>

<p align="center">
  <img alt="Platform" src="https://img.shields.io/badge/MCU-LOLIN%20S2%20Mini%20%2F%20ESP32--S2-111827">
  <img alt="Firmware" src="https://img.shields.io/badge/Firmware-Arduino%20C%2B%2B-facc15">
  <img alt="Control" src="https://img.shields.io/badge/Control-Local%20Wi--Fi-ef4444">
  <img alt="PCB" src="https://img.shields.io/badge/PCB-Rev%20A%20Prototype-facc15">
  <img alt="Software license" src="https://img.shields.io/badge/Software-Apache--2.0-blue">
  <img alt="Hardware license" src="https://img.shields.io/badge/Hardware-CERN--OHL--S--2.0-red">
</p>

> **Prototype / safety notice.** This repository contains hobby-grade firmware and a preliminary carrier PCB. The PCB has not been verified by a native KiCad Design Rules Check (DRC) or by physical bench testing in this environment. It is not a production-certified product and should not be fabricated or operated until you complete the verification checklist in this README and `hardware/CyberCar_SPARK_Redline_Carrier/FABRICATION_AND_REVIEW.md`.

## 1. Project at a glance

CyberCar SPARK REDLINE is a small four-wheel chassis controlled by a LOLIN S2 Mini (ESP32-S2). The ESP32 creates its own Wi-Fi access point and serves a locally hosted browser dashboard. A phone joins the car's network and sends commands directly to the ESP32; no home router, cloud service, or Internet uplink is required for this local-control configuration.

The electrical architecture keeps high motor current away from the carrier PCB: an external L298N module drives the motors, while a separate buck converter makes regulated 5 V for the ESP32/accessories. The carrier is an interconnect and accessory board, not a replacement for the ESP32, L298N module, or buck converter.

### Current implementation vs. planned features

| Capability | Status in this release |
|---|---|
| ESP32-created Wi-Fi AP and local captive-portal-style page | Implemented in firmware; phone OS auto-opening varies by device, so `http://192.168.4.1/` can be entered manually. |
| PIN-gated control session | Implemented for the local HTTP dashboard. |
| Manual forward / backward / left / right / stop | Implemented using GPIO direction and enable signals. |
| FAST / MEDIUM / SLOW profiles without ESP32 LEDC hardware PWM | Implemented; MEDIUM/SLOW are timed digital on/off bursts and can feel jerky. |
| Latched emergency-brake command and resume action | Implemented in software; not a safety-rated physical brake. |
| Servo accessory, buzzer and status LEDs | Firmware support is present; verify wiring and optional-library setup. |
| Face images change with robot command/state | Implemented by serving small JPEG files from flash; browser renders them. |
| Native-distance-sensor support | **Not present in this version.** No ultrasonic sensor is included in the firmware or carrier connector plan. |
| Autonomous navigation / trained reinforcement-learning controller | **Not implemented as a driving policy.** UI concepts/art assets are not proof of a trained model. |
| Cloud or public-Internet control | Not implemented. The AP is a local network. |
| Fabrication-ready PCB | Not yet. The Rev-A KiCad source is a reviewable draft; run KiCad DRC and bench checks first. |

## 2. Repository map

```text
CyberCar_SPARK_Redline_Studio/
├── README.md                         Detailed project guide (this file)
├── LICENSE                           Full Apache License 2.0 text for covered software/docs
├── LICENSE-HARDWARE                  Full CERN-OHL-S-2.0 text for covered PCB source
├── LICENSE-HARDWARE-NOTICE.md        Scope/source-location note for the PCB license
├── LICENSE-OVERVIEW.md               How license scopes are separated
├── NOTICE.md                         Attribution, contributor and asset notes
├── LICENSE-IMAGES-AND-ASSETS.md       Notes about photos, faces, fonts and other media
├── SECURE_BACKEND.md                 API validation and security design notes
├── WIRING.md                         Hardware hookup overview
├── HEADER_FILES_GUIDE.md             How the C++ headers are organized
├── FACE_ASSETS_GUIDE.md              Face asset workflow and memory notes
├── BUILD_AND_MEMORY_NOTES.md         Build checklist and flash/RAM guidance
├── CyberCar/CyberCarPro_optimized/
│   ├── CyberCarPro_optimized.ino     Firmware entry point and controller classes
│   ├── config.h                      constexpr hardware/network/time configuration
│   ├── motor_driver.h                GPIO motor driver and drive profiles
│   ├── captive_portal.h              AP, dashboard, HTTP handlers, authentication flow
│   ├── secure_backend.h              Parsing, headers, origin checks, JSON helper
│   └── face_assets.h                 Compressed JPEG bytes stored in flash (PROGMEM)
├── original_uploaded_face_images/    User-supplied source images; check rights before redistributing
├── docs/
│   ├── CyberCar_SPARK_Redline_Preview.html
│   ├── CyberCar_SPARK_Redline_Wiring.png
│   ├── CyberCar_SPARK_Redline_Wiring.svg
│   ├── assets/readme/                Dashboard and controls/team screenshots
│   └── assets/faces/                 Optimized/sample face assets
└── hardware/CyberCar_SPARK_Redline_Carrier/
    ├── CyberCar_SPARK_Redline_Carrier.kicad_pcb   Editable board source
    ├── CyberCar_SPARK_Redline_Carrier.kicad_pro   KiCad project settings
    ├── pcb_layout_preview.png/.svg                 Annotated design preview
    ├── carrier_system_schematic.png/.svg           System wiring explanation (not native KiCad schematic)
    ├── BOM.csv                                     Bill of materials
    ├── CONNECTOR_PINOUT.csv                        Connector map
    ├── README.md                                   PCB-specific instructions
    └── FABRICATION_AND_REVIEW.md                   Pre-fabrication review checklist
```

## 3. Cyberpunk dashboard preview

The live dashboard uses black surfaces, hazard-yellow highlights, and signal-red warning states. It includes a control PIN gate, manual joystick, drive profiles, live state display, face art, auxiliary actions, emergency stop/resume, and SPARK Team cards.

### Dashboard

<img width="1440" height="980" alt="portal-dashboard" src="https://github.com/user-attachments/assets/a543a0ee-623d-403d-a356-bc2c5a341c3b" />


### Manual controls and SPARK Team

<img width="1440" height="907" alt="portal-controls-team" src="https://github.com/user-attachments/assets/974154ce-6213-47b1-9a7a-bac9ec1413dc" />


### Run the preview

Open [`docs/CyberCar_SPARK_Redline_Preview.html`](docs/CyberCar_SPARK_Redline_Preview.html) in a desktop or mobile browser. This page is a **local UI simulation**: it does not send commands to the physical car. The actual dashboard is embedded in `captive_portal.h` and served by the ESP32.

The phone's operating system may or may not automatically display a captive-portal prompt. If it does not, join the car's AP and type the address below directly into the browser.

## 4. SPARK Team

| Team member | Role | Discipline |
|---|---|---|
| **Haider Nather** | Team Lead | AI & Robotics Engineering |
| **Ahmed Amjed** | AI Developer | AI & Robotics Engineering |
| **Ayat Sayed** | System Designer | AI & Robotics Engineering |
| **Baraa Fadhel** | Robotics Designer | AI & Robotics Engineering |

### Suggested project ownership

- **Team Lead — Haider Nather:** integration plan, review checklist, release coordination and tracking issues across hardware/software.
- **AI Developer — Ahmed Amjed:** future learning, perception or decision-making experiments; define datasets, baselines and evaluation before claiming an AI feature is operational.
- **System Designer — Ayat Sayed:** interface requirements, subsystem boundaries, dashboard states, API contract and user experience.
- **Robotics Designer — Baraa Fadhel:** chassis, motor/mechanical mounting, wiring harness, PCB fit, component placement and physical test setup.

These assignments are project documentation and may be adjusted by the team. Use only approved photographs/likenesses when adding member profile images. Dashboard labels such as “AI” and “Q-learning” are visual/product organization elements; the current firmware does not include a trained driving policy.

## 5. System architecture

```text
Phone / tablet
  └─ joins Cyber-Car-Pro (local Wi-Fi AP)
      └─ HTTP dashboard at 192.168.4.1
          ├─ authenticated command routes
          └─ status polling
              │
              ▼
       LOLIN S2 Mini / ESP32-S2
          ├─ IN1..IN4: motor direction
          ├─ ENA/ENB: motor-driver enable
          ├─ GPIO13: optional servo signal
          ├─ GPIO18: buzzer driver
          └─ GPIO8 / GPIO9 / GPIO3: R/G/B indicators
              │
              ├─ GPIO harness ──> external L298N logic inputs
              │                    └─ L298N outputs ──> two DC motors
              └─ low-current carrier accessories (LEDs, buzzer, servo rail)
```

This setup is a **local Wi-Fi controller**, not an Internet gateway. A phone may show “Connected without Internet”; that is expected. Do not port-forward the HTTP dashboard to the public Internet.

## 6. Hardware inventory

| Part | Role | Engineering notes |
|---|---|---|
| LOLIN S2 Mini / ESP32-S2 | Wi-Fi AP, HTTP server, command handling and logic GPIO | GPIO is 3.3 V logic; do not apply 5 V to a GPIO. Board pinout must be checked against the exact revision. |
| L298N dual H-bridge module | Reverses motor polarity and enables/disables each motor channel | Motor voltage/current and module regulator/jumpers vary by board; verify your exact module. |
| Two DC gear motors | Drive the chassis | Motor OUT terminals remain on the L298N module, not the carrier PCB. |
| 2S LiPo battery | Supplies motor power and buck input | Nominal 7.4 V, 8.4 V fully charged. Use a suitable balance charger and battery protection practices. |
| 5 V buck converter | Regulated accessory/ESP32 supply | Measure output before connection; set to 5.00 V. Size for servo peak/stall load as well as MCU and accessories. |
| Servo | Optional mechanism | Separate 5 V rail capable of its peak current; share GND with ESP32. Never power servo motor current from a GPIO. |
| R/G/B LEDs with resistors | Visual status | Each LED channel needs its own series resistor. Confirm polarity. |
| Passive piezo buzzer with NPN driver | Audio feedback | Check transistor pin order, resistor values and buzzer polarity/type. |
| Carrier PCB Rev A | Connectorization and low-current accessories | Does not carry motor stall current and has no battery charger/BMS or safety-rated power cut-off. |
| Future expansion connector J5 | Replacement peripheral/sensor interface | No sensor is currently included. GPIO1/GPIO2 are not 5 V tolerant; pick future components accordingly. |

## 7. Wiring reference

![CyberCar SPARK REDLINE wiring diagram](docs/CyberCar_SPARK_Redline_Wiring.png)
<svg xmlns="http://www.w3.org/2000/svg" width="1240" height="1020" viewBox="0 0 1240 1020"><defs><linearGradient id="pcb" x1="0" y1="0" x2="1" y2="1"><stop offset="0" stop-color="#171a1f"/><stop offset="1" stop-color="#07090c"/></linearGradient><pattern id="grid" width="10" height="10" patternUnits="userSpaceOnUse"><path d="M10 0H0V10" fill="none" stroke="#22282f" stroke-width=".5"/></pattern></defs><rect width="1240" height="1020" fill="#080a0d"/><rect x="70" y="70" width="1100.0" height="880.0" rx="12" fill="url(#pcb)" stroke="#d4a600" stroke-width="3"/><rect x="70" y="70" width="1100.0" height="880.0" fill="url(#grid)" opacity=".4"/><path d="M210 810 l25 -25" stroke="#264c4b" stroke-width="1" opacity=".4"/><path d="M290 810 l25 -25" stroke="#264c4b" stroke-width="1" opacity=".4"/><path d="M370 810 l25 -25" stroke="#264c4b" stroke-width="1" opacity=".4"/><path d="M450 810 l25 -25" stroke="#264c4b" stroke-width="1" opacity=".4"/><path d="M530 810 l25 -25" stroke="#264c4b" stroke-width="1" opacity=".4"/><path d="M610 810 l25 -25" stroke="#264c4b" stroke-width="1" opacity=".4"/><path d="M690 810 l25 -25" stroke="#264c4b" stroke-width="1" opacity=".4"/><path d="M770 810 l25 -25" stroke="#264c4b" stroke-width="1" opacity=".4"/><path d="M850 810 l25 -25" stroke="#264c4b" stroke-width="1" opacity=".4"/><path d="M930 810 l25 -25" stroke="#264c4b" stroke-width="1" opacity=".4"/><path d="M1010 810 l25 -25" stroke="#264c4b" stroke-width="1" opacity=".4"/><path d="M1090 810 l25 -25" stroke="#264c4b" stroke-width="1" opacity=".4"/><line x1="290" y1="356.2" x2="1000" y2="330.8" stroke="#ff424b" stroke-width="4.0" stroke-linecap="round" opacity=".92"/><line x1="290" y1="381.59999999999997" x2="1000" y2="381.6" stroke="#ff424b" stroke-width="4.0" stroke-linecap="round" opacity=".92"/><line x1="290" y1="406.99999999999994" x2="1000" y2="432.40000000000003" stroke="#ff424b" stroke-width="4.0" stroke-linecap="round" opacity=".92"/><line x1="290" y1="432.4" x2="1000" y2="483.2" stroke="#ff424b" stroke-width="4.0" stroke-linecap="round" opacity=".92"/><line x1="290" y1="457.8" x2="1000" y2="534.0" stroke="#ff424b" stroke-width="4.0" stroke-linecap="round" opacity=".92"/><line x1="290" y1="483.19999999999993" x2="1000" y2="584.8000000000001" stroke="#ff424b" stroke-width="4.0" stroke-linecap="round" opacity=".92"/><line x1="290" y1="508.6" x2="1000" y2="725.4000000000001" stroke="#31d8e8" stroke-width="4.0" stroke-linecap="round" opacity=".92"/><line x1="290" y1="533.9999999999999" x2="396.9" y2="534.0" stroke="#ff424b" stroke-width="3.5" stroke-linecap="round" opacity=".92"/><line x1="473.1" y1="534.0" x2="501.29999999999995" y2="534.0" stroke="#ff424b" stroke-width="3.5" stroke-linecap="round" opacity=".92"/><line x1="290" y1="559.4" x2="396.9" y2="559.4" stroke="#ff424b" stroke-width="3.5" stroke-linecap="round" opacity=".92"/><line x1="473.1" y1="559.4" x2="501.29999999999995" y2="559.4" stroke="#ff424b" stroke-width="3.5" stroke-linecap="round" opacity=".92"/><line x1="290" y1="584.8" x2="396.9" y2="584.8" stroke="#ff424b" stroke-width="3.5" stroke-linecap="round" opacity=".92"/><line x1="473.1" y1="584.8" x2="501.29999999999995" y2="584.8" stroke="#ff424b" stroke-width="3.5" stroke-linecap="round" opacity=".92"/><line x1="290" y1="610.1999999999999" x2="350.0" y2="610.2" stroke="#ff424b" stroke-width="3.5" stroke-linecap="round" opacity=".92"/><line x1="350.0" y1="610.2" x2="350.0" y2="645.0" stroke="#ff424b" stroke-width="3.5" stroke-linecap="round" opacity=".92"/><line x1="350.0" y1="645.0" x2="461.9" y2="645.0" stroke="#ff424b" stroke-width="3.5" stroke-linecap="round" opacity=".92"/><line x1="538.1" y1="645.0" x2="624.6" y2="650.0" stroke="#ff424b" stroke-width="3.5" stroke-linecap="round" opacity=".92"/><line x1="624.6" y1="650.0" x2="630.0" y2="691.9" stroke="#ff424b" stroke-width="3.5" stroke-linecap="round" opacity=".92"/><line x1="650.0" y1="650.0" x2="940.0" y2="650.0" stroke="#31d8e8" stroke-width="4.0" stroke-linecap="round" opacity=".92"/><line x1="940.0" y1="650.0" x2="940.0" y2="190.0" stroke="#31d8e8" stroke-width="4.0" stroke-linecap="round" opacity=".92"/><line x1="940.0" y1="190.0" x2="915.4000000000001" y2="190.0" stroke="#31d8e8" stroke-width="4.0" stroke-linecap="round" opacity=".92"/><line x1="290" y1="635.6" x2="1000" y2="832.6999999999999" stroke="#ff424b" stroke-width="3.5" stroke-linecap="round" opacity=".92"/><line x1="290" y1="661.0" x2="1000" y2="858.1" stroke="#ff424b" stroke-width="3.5" stroke-linecap="round" opacity=".92"/><line x1="290" y1="330.79999999999995" x2="150.0" y2="330.79999999999995" stroke="#31d8e8" stroke-width="3.5" stroke-linecap="round" opacity=".92"/><line x1="150.0" y1="330.79999999999995" x2="150.0" y2="781.9" stroke="#31d8e8" stroke-width="3.5" stroke-linecap="round" opacity=".92"/><line x1="150.0" y1="781.9" x2="1000" y2="781.9" stroke="#31d8e8" stroke-width="3.5" stroke-linecap="round" opacity=".92"/><line x1="169.60000000000002" y1="190.0" x2="110.0" y2="190.0" stroke="#31d8e8" stroke-width="8.0" stroke-linecap="round" opacity=".92"/><line x1="110.0" y1="190.0" x2="110.0" y2="305.4" stroke="#31d8e8" stroke-width="8.0" stroke-linecap="round" opacity=".92"/><line x1="110.0" y1="305.4" x2="290" y2="305.4" stroke="#31d8e8" stroke-width="8.0" stroke-linecap="round" opacity=".92"/><line x1="169.60000000000002" y1="190.0" x2="169.60000000000002" y2="225.0" stroke="#ff424b" stroke-width="8.0" stroke-linecap="round" opacity=".92"/><line x1="169.60000000000002" y1="225.0" x2="1100.0" y2="225.0" stroke="#ff424b" stroke-width="8.0" stroke-linecap="round" opacity=".92"/><line x1="1100.0" y1="225.0" x2="1100.0" y2="700.0" stroke="#ff424b" stroke-width="8.0" stroke-linecap="round" opacity=".92"/><line x1="1100.0" y1="700.0" x2="1000" y2="700.0" stroke="#ff424b" stroke-width="8.0" stroke-linecap="round" opacity=".92"/><line x1="864.5999999999999" y1="225.0" x2="864.5999999999999" y2="190.0" stroke="#ff424b" stroke-width="8.0" stroke-linecap="round" opacity=".92"/><line x1="855.0" y1="880.0" x2="835.0" y2="880.0" stroke="#ff424b" stroke-width="6.0" stroke-linecap="round" opacity=".92"/><line x1="835.0" y1="880.0" x2="835.0" y2="905.0" stroke="#ff424b" stroke-width="6.0" stroke-linecap="round" opacity=".92"/><line x1="774.6000000000001" y1="880.0" x2="755.0" y2="880.0" stroke="#ff424b" stroke-width="6.0" stroke-linecap="round" opacity=".92"/><line x1="755.0" y1="880.0" x2="755.0" y2="905.0" stroke="#ff424b" stroke-width="6.0" stroke-linecap="round" opacity=".92"/><line x1="755.0" y1="905.0" x2="1070.0" y2="905.0" stroke="#ff424b" stroke-width="6.0" stroke-linecap="round" opacity=".92"/><line x1="1070.0" y1="905.0" x2="1070.0" y2="700.0" stroke="#ff424b" stroke-width="6.0" stroke-linecap="round" opacity=".92"/><circle cx="169.60000000000002" cy="190.0" r="10.56" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="169.60000000000002" cy="190.0" r="3.84" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="220.39999999999998" cy="190.0" r="10.56" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="220.39999999999998" cy="190.0" r="3.84" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="290" cy="280.0" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="290" cy="280.0" r="2.88" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><text x="276" y="284.0" fill="#fff1a1" text-anchor="end" font-family="Arial" font-size="11">1</text><circle cx="290" cy="305.4" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="290" cy="305.4" r="2.88" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><text x="276" y="309.4" fill="#fff1a1" text-anchor="end" font-family="Arial" font-size="11">2</text><circle cx="290" cy="330.79999999999995" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="290" cy="330.79999999999995" r="2.88" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><text x="276" y="334.79999999999995" fill="#fff1a1" text-anchor="end" font-family="Arial" font-size="11">3</text><circle cx="290" cy="356.2" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="290" cy="356.2" r="2.88" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><text x="276" y="360.2" fill="#fff1a1" text-anchor="end" font-family="Arial" font-size="11">4</text><circle cx="290" cy="381.59999999999997" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="290" cy="381.59999999999997" r="2.88" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><text x="276" y="385.59999999999997" fill="#fff1a1" text-anchor="end" font-family="Arial" font-size="11">5</text><circle cx="290" cy="406.99999999999994" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="290" cy="406.99999999999994" r="2.88" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><text x="276" y="410.99999999999994" fill="#fff1a1" text-anchor="end" font-family="Arial" font-size="11">6</text><circle cx="290" cy="432.4" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="290" cy="432.4" r="2.88" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><text x="276" y="436.4" fill="#fff1a1" text-anchor="end" font-family="Arial" font-size="11">7</text><circle cx="290" cy="457.8" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="290" cy="457.8" r="2.88" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><text x="276" y="461.8" fill="#fff1a1" text-anchor="end" font-family="Arial" font-size="11">8</text><circle cx="290" cy="483.19999999999993" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="290" cy="483.19999999999993" r="2.88" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><text x="276" y="487.19999999999993" fill="#fff1a1" text-anchor="end" font-family="Arial" font-size="11">9</text><circle cx="290" cy="508.6" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="290" cy="508.6" r="2.88" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><text x="276" y="512.6" fill="#fff1a1" text-anchor="end" font-family="Arial" font-size="11">10</text><circle cx="290" cy="533.9999999999999" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="290" cy="533.9999999999999" r="2.88" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><text x="276" y="537.9999999999999" fill="#fff1a1" text-anchor="end" font-family="Arial" font-size="11">11</text><circle cx="290" cy="559.4" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="290" cy="559.4" r="2.88" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><text x="276" y="563.4" fill="#fff1a1" text-anchor="end" font-family="Arial" font-size="11">12</text><circle cx="290" cy="584.8" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="290" cy="584.8" r="2.88" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><text x="276" y="588.8" fill="#fff1a1" text-anchor="end" font-family="Arial" font-size="11">13</text><circle cx="290" cy="610.1999999999999" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="290" cy="610.1999999999999" r="2.88" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><text x="276" y="614.1999999999999" fill="#fff1a1" text-anchor="end" font-family="Arial" font-size="11">14</text><circle cx="290" cy="635.6" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="290" cy="635.6" r="2.88" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><text x="276" y="639.6" fill="#fff1a1" text-anchor="end" font-family="Arial" font-size="11">15</text><circle cx="290" cy="661.0" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="290" cy="661.0" r="2.88" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><text x="276" y="665.0" fill="#fff1a1" text-anchor="end" font-family="Arial" font-size="11">16</text><circle cx="1000" cy="280.0" r="10.56" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="1000" cy="280.0" r="3.84" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="1000" cy="330.8" r="10.56" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="1000" cy="330.8" r="3.84" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="1000" cy="381.6" r="10.56" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="1000" cy="381.6" r="3.84" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="1000" cy="432.40000000000003" r="10.56" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="1000" cy="432.40000000000003" r="3.84" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="1000" cy="483.2" r="10.56" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="1000" cy="483.2" r="3.84" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="1000" cy="534.0" r="10.56" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="1000" cy="534.0" r="3.84" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="1000" cy="584.8000000000001" r="10.56" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="1000" cy="584.8000000000001" r="3.84" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="1000" cy="674.6" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="1000" cy="674.6" r="2.88" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="1000" cy="700.0" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="1000" cy="700.0" r="2.88" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="1000" cy="725.4000000000001" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="1000" cy="725.4000000000001" r="2.88" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="1000" cy="781.9" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="1000" cy="781.9" r="2.88" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="1000" cy="807.3000000000001" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="1000" cy="807.3000000000001" r="2.88" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="1000" cy="832.6999999999999" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="1000" cy="832.6999999999999" r="2.88" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="1000" cy="858.1" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="1000" cy="858.1" r="2.88" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="864.5999999999999" cy="190.0" r="10.56" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="864.5999999999999" cy="190.0" r="3.84" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="915.4000000000001" cy="190.0" r="10.56" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="915.4000000000001" cy="190.0" r="3.84" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="396.9" cy="534.0" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="396.9" cy="534.0" r="2.56" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="473.1" cy="534.0" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="473.1" cy="534.0" r="2.56" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="396.9" cy="559.4" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="396.9" cy="559.4" r="2.56" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="473.1" cy="559.4" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="473.1" cy="559.4" r="2.56" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="396.9" cy="584.8" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="396.9" cy="584.8" r="2.56" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="473.1" cy="584.8" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="473.1" cy="584.8" r="2.56" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="501.29999999999995" cy="534.0" r="8.8" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="501.29999999999995" cy="534.0" r="2.56" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="526.7" cy="534.0" r="8.8" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="526.7" cy="534.0" r="2.56" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="501.29999999999995" cy="559.4" r="8.8" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="501.29999999999995" cy="559.4" r="2.56" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="526.7" cy="559.4" r="8.8" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="526.7" cy="559.4" r="2.56" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="501.29999999999995" cy="584.8" r="8.8" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="501.29999999999995" cy="584.8" r="2.56" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="526.7" cy="584.8" r="8.8" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="526.7" cy="584.8" r="2.56" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="461.9" cy="645.0" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="461.9" cy="645.0" r="2.56" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="538.1" cy="645.0" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="538.1" cy="645.0" r="2.56" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="630.0" cy="691.9" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="630.0" cy="691.9" r="2.56" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="630.0" cy="768.1" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="630.0" cy="768.1" r="2.56" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="624.6" cy="650.0" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="624.6" cy="650.0" r="2.56" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="650.0" cy="650.0" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="650.0" cy="650.0" r="2.56" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="675.4" cy="650.0" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="675.4" cy="650.0" r="2.56" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="855.0" cy="880.0" r="9.24" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="855.0" cy="880.0" r="3.2" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="880.0" cy="880.0" r="9.24" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="880.0" cy="880.0" r="3.2" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="774.6000000000001" cy="880.0" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="774.6000000000001" cy="880.0" r="2.56" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="800.0" cy="880.0" r="7.92" fill="#d6a900" stroke="#fbe76b" stroke-width="2"/><circle cx="800.0" cy="880.0" r="2.56" fill="#05070a" stroke="#e1e6ec" stroke-width="1"/><circle cx="100" cy="100" r="16" fill="#07090c" stroke="#bec3c9" stroke-width="2"/><circle cx="100" cy="100" r="11" fill="#07090c" stroke="#f3c833" stroke-width="2"/><circle cx="1140" cy="100" r="16" fill="#07090c" stroke="#bec3c9" stroke-width="2"/><circle cx="1140" cy="100" r="11" fill="#07090c" stroke="#f3c833" stroke-width="2"/><circle cx="100" cy="920" r="16" fill="#07090c" stroke="#bec3c9" stroke-width="2"/><circle cx="100" cy="920" r="11" fill="#07090c" stroke="#f3c833" stroke-width="2"/><circle cx="1140" cy="920" r="16" fill="#07090c" stroke="#bec3c9" stroke-width="2"/><circle cx="1140" cy="920" r="11" fill="#07090c" stroke="#f3c833" stroke-width="2"/><text x="620" y="118.0" fill="#ffd400" font-family="Arial" font-size="18.0" text-anchor="middle" font-weight="bold">SPARK REDLINE  /  CARRIER REV A</text><text x="310" y="151.0" fill="#ffd400" font-family="Arial" font-size="9.5" text-anchor="middle" font-weight="bold">5V FROM REGULATED BUCK ONLY</text><text x="255.0" y="235.0" fill="#ffd400" font-family="Arial" font-size="9.5" text-anchor="middle" font-weight="bold">ESP32-S2 MINI HARNESS - 1x16 / 2.54mm</text><text x="1000" y="209.0" fill="#ffd400" font-family="Arial" font-size="9.5" text-anchor="middle" font-weight="bold">L298N CONTROL ONLY - NO MOTOR CURRENT</text><text x="910" y="633.0" fill="#ffd400" font-family="Arial" font-size="9.0" text-anchor="middle" font-weight="bold">SERVO</text><text x="1000" y="749.0" fill="#ffd400" font-family="Arial" font-size="8.0" text-anchor="middle" font-weight="bold">FUTURE I/O - 3V3 LOGIC ONLY</text><text x="620" y="930.0" fill="#ffd400" font-family="Arial" font-size="8.0" text-anchor="middle" font-weight="bold">SPARK TEAM</text><text x="390" y="194.0" fill="#ffd400" font-family="Arial" font-size="8.0" text-anchor="middle" font-weight="bold">RATED 5V INPUT</text><text x="180.0" y="283.0" fill="#ffe66e" font-family="Arial" font-size="6.4799999999999995" text-anchor="end">01 GND</text><text x="180.0" y="308.4" fill="#ffe66e" font-family="Arial" font-size="6.4799999999999995" text-anchor="end">02 +5V IN</text><text x="180.0" y="333.79999999999995" fill="#ffe66e" font-family="Arial" font-size="6.4799999999999995" text-anchor="end">03 3V3 OUT</text><text x="180.0" y="359.2" fill="#ffe66e" font-family="Arial" font-size="6.4799999999999995" text-anchor="end">04 GPIO7 / IN1</text><text x="180.0" y="384.59999999999997" fill="#ffe66e" font-family="Arial" font-size="6.4799999999999995" text-anchor="end">05 GPIO6 / IN2</text><text x="180.0" y="410.0" fill="#ffe66e" font-family="Arial" font-size="6.4799999999999995" text-anchor="end">06 GPIO4 / IN3</text><text x="180.0" y="435.40000000000003" fill="#ffe66e" font-family="Arial" font-size="6.4799999999999995" text-anchor="end">07 GPIO5 / IN4</text><text x="180.0" y="460.8" fill="#ffe66e" font-family="Arial" font-size="6.4799999999999995" text-anchor="end">08 GPIO11 / ENA</text><text x="180.0" y="486.2" fill="#ffe66e" font-family="Arial" font-size="6.4799999999999995" text-anchor="end">09 GPIO12 / ENB</text><text x="180.0" y="511.6" fill="#ffe66e" font-family="Arial" font-size="6.4799999999999995" text-anchor="end">10 GPIO13 / SERVO</text><text x="180.0" y="537.0" fill="#ffe66e" font-family="Arial" font-size="6.4799999999999995" text-anchor="end">11 GPIO8 / RED</text><text x="180.0" y="562.4" fill="#ffe66e" font-family="Arial" font-size="6.4799999999999995" text-anchor="end">12 GPIO9 / GREEN</text><text x="180.0" y="587.8000000000001" fill="#ffe66e" font-family="Arial" font-size="6.4799999999999995" text-anchor="end">13 GPIO3 / BLUE</text><text x="180.0" y="613.2" fill="#ffe66e" font-family="Arial" font-size="6.4799999999999995" text-anchor="end">14 GPIO18 / BUZZ</text><text x="180.0" y="638.6" fill="#ffe66e" font-family="Arial" font-size="6.4799999999999995" text-anchor="end">15 GPIO1 / AUX</text><text x="180.0" y="664.0" fill="#ffe66e" font-family="Arial" font-size="6.4799999999999995" text-anchor="end">16 GPIO2 / AUX</text><text x="195.0" y="178.0" fill="#fff1a1" font-family="Arial" font-size="13" text-anchor="middle">J1</text><text x="290" y="458.5" fill="#fff1a1" font-family="Arial" font-size="13" text-anchor="middle">J2</text><text x="1000" y="420.40000000000003" fill="#fff1a1" font-family="Arial" font-size="13" text-anchor="middle">J3</text><text x="1000" y="688.0" fill="#fff1a1" font-family="Arial" font-size="13" text-anchor="middle">J4</text><text x="1000" y="808.0" fill="#fff1a1" font-family="Arial" font-size="13" text-anchor="middle">J5</text><text x="890.0" y="178.0" fill="#fff1a1" font-family="Arial" font-size="13" text-anchor="middle">J6</text><rect x="415.0" y="522.0" width="40" height="24" rx="4" fill="#20262b" stroke="#ffd400" stroke-width="2"/><text x="435.0" y="538.0" fill="#ffe66e" font-family="Arial" font-size="11" text-anchor="middle">R3</text><rect x="415.0" y="547.4" width="40" height="24" rx="4" fill="#20262b" stroke="#ffd400" stroke-width="2"/><text x="435.0" y="563.4" fill="#ffe66e" font-family="Arial" font-size="11" text-anchor="middle">R4</text><rect x="415.0" y="572.8" width="40" height="24" rx="4" fill="#20262b" stroke="#ffd400" stroke-width="2"/><text x="435.0" y="588.8" fill="#ffe66e" font-family="Arial" font-size="11" text-anchor="middle">R5</text><ellipse cx="514.0" cy="534.0" rx="18" ry="10" fill="#ff4141" opacity=".25" stroke="#ff4141" stroke-width="2"/><text x="514.0" y="522.0" fill="#ff4141" font-family="Arial" font-size="11" text-anchor="middle">D1</text><ellipse cx="514.0" cy="559.4" rx="18" ry="10" fill="#5cff83" opacity=".25" stroke="#5cff83" stroke-width="2"/><text x="514.0" y="547.4" fill="#5cff83" font-family="Arial" font-size="11" text-anchor="middle">D2</text><ellipse cx="514.0" cy="584.8" rx="18" ry="10" fill="#36cfff" opacity=".25" stroke="#36cfff" stroke-width="2"/><text x="514.0" y="572.8" fill="#36cfff" font-family="Arial" font-size="11" text-anchor="middle">D3</text><rect x="480.0" y="633.0" width="40" height="24" rx="4" fill="#20262b" stroke="#ffd400" stroke-width="2"/><text x="500.0" y="649.0" fill="#ffe66e" font-family="Arial" font-size="11" text-anchor="middle">R1</text><rect x="610.0" y="718.0" width="40" height="24" rx="4" fill="#20262b" stroke="#ffd400" stroke-width="2"/><text x="630.0" y="734.0" fill="#ffe66e" font-family="Arial" font-size="11" text-anchor="middle">R2</text><rect x="630.0" y="638.0" width="40" height="24" rx="4" fill="#20262b" stroke="#ffd400" stroke-width="2"/><text x="650.0" y="654.0" fill="#ffe66e" font-family="Arial" font-size="11" text-anchor="middle">Q1</text><rect x="847.5" y="868.0" width="40" height="24" rx="4" fill="#20262b" stroke="#ffd400" stroke-width="2"/><text x="867.5" y="884.0" fill="#ffe66e" font-family="Arial" font-size="11" text-anchor="middle">C1</text><rect x="767.3000000000001" y="868.0" width="40" height="24" rx="4" fill="#20262b" stroke="#ffd400" stroke-width="2"/><text x="787.3000000000001" y="884.0" fill="#ffe66e" font-family="Arial" font-size="11" text-anchor="middle">C2</text><text x="620" y="968" fill="#aab2bc" font-family="Arial" font-size="12" text-anchor="middle">Preview only — press B to refill the GND zone in KiCad, then run DRC</text></svg>
- [Editable SVG wiring diagram](docs/CyberCar_SPARK_Redline_Wiring.svg)
- [PCB layout preview](hardware/CyberCar_SPARK_Redline_Carrier/pcb_layout_preview.png)
- [System wiring explanation](hardware/CyberCar_SPARK_Redline_Carrier/carrier_system_schematic.png)
- [CONNECTOR_PINOUT.csv](https://github.com/user-attachments/files/33843589/CONNECTOR_PINOUT.csv)
- [BOM.csv](https://github.com/us<img width="1240" height="1020" alt="pcb_layout_preview" src="https://github.com/user-attachments/assets/6a7d4ed9-f02f-440d-919b-9dadb81b6fd3" />
er-attachments/files/33843572/BOM.csv)


### Critical power rules

1. **Carrier J1 accepts regulated +5.00 V only.** Never connect raw 2S LiPo voltage to J1, the ESP32 5 V/VBUS pin or any GPIO.
2. Route battery positive/negative to the L298N motor supply with suitable wire, connector and fuse/protection. Do not send motor stall current through this carrier board or the ESP32.
3. Connect grounds together: battery negative, buck negative, ESP32 GND, L298N GND, carrier GND and servo GND.
4. On the L298N module, remove the ENA and ENB jumper caps when GPIO11/GPIO12 control the enable inputs.
5. Leave the L298N module's `+5V` terminal disconnected from carrier +5 V unless the documentation for your exact module explicitly requires another connection. L298N boards use different regulator/jumper arrangements.
6. Avoid powering the controller simultaneously from USB and the carrier's +5 V path unless the exact board's power-path isolation has been verified. During early testing, use one supply source at a time.
7. Provide a physical battery disconnect and suitable battery protection. Browser controls, software brake commands and watchdogs are not safety-rated emergency isolation.
8. The future accessory header is 3.3 V logic only. Verify any new replacement component's supply range, IO voltage, current draw, signal timing and common-ground requirements before wiring.

## 8. Pin map (firmware `config.h`)

| ESP32-S2 GPIO | Board/carrier signal | External destination |
|---:|---|---|
| GPIO7 | `MOTOR_IN1_PIN` | L298N IN1 |
| GPIO6 | `MOTOR_IN2_PIN` | L298N IN2 |
| GPIO4 | `MOTOR_IN3_PIN` | L298N IN3 |
| GPIO5 | `MOTOR_IN4_PIN` | L298N IN4 |
| GPIO11 | `MOTOR_ENA_PIN` | L298N ENA; remove ENA jumper |
| GPIO12 | `MOTOR_ENB_PIN` | L298N ENB; remove ENB jumper |
| GPIO13 | `BALL_SERVO_PIN` | Optional servo signal |
| GPIO8 | `LED_RED_PIN` | Red LED through its resistor |
| GPIO9 | `LED_GREEN_PIN` | Green LED through its resistor |
| GPIO3 | `LED_BLUE_PIN` | Blue LED through its resistor |
| GPIO18 | `BUZZER_PIN` | NPN buzzer driver input via resistor |
| GPIO1 | Expansion | Future 3.3 V-safe peripheral only |
| GPIO2 | Expansion | Future 3.3 V-safe peripheral only |

The pin map is also represented in `CONNECTOR_PINOUT.csv`. If you change a GPIO, update `config.h`, the PCB harness table, the wiring illustration, firmware comments and README together.

## 9. Carrier PCB design — Rev A

<img width="1400" height="1152" alt="pcb_layout_preview" src="https://github.com/user-attachments/assets/9ba72bb5-204a-4801-a2c2-583ab1e5e23b" />


The included board is an editable **two-layer low-current carrier PCB**. It is meant to simplify wiring and mount accessory circuitry; it does not integrate the ESP32-S2 module, L298N power bridge or the buck converter.

### Files in the KiCad project

| File | Purpose |
|---|---|
| `CyberCar_SPARK_Redline_Carrier.kicad_pcb` | Editable board layout: edge outline, footprints, net names, copper tracks and GND zone definition. |
| `CyberCar_SPARK_Redline_Carrier.kicad_pro` | KiCad project settings. |
| `pcb_layout_preview.svg/.png` | Annotated visual overview for documentation; not Gerber manufacturing output. |
| `carrier_system_schematic.svg/.png` | System-level wiring illustration; **not a native KiCad schematic file**. |
| `BOM.csv` | Proposed component list and notes. |
| `CONNECTOR_PINOUT.csv` | Header/terminal pin map. |
| `FABRICATION_AND_REVIEW.md` | Design-review, safety and fabrication checklist. |

### Preliminary mechanical/electrical assumptions

- Nominal board outline: approximately 110 × 88 mm as marked by `Edge.Cuts`.
- Two copper layers, nominal 1.6 mm FR-4.
- Four M3 mounting-hole positions.
- 5 V and signal interconnects only; no high-current motor power copper on this carrier.
- J1: regulated +5 V/GND input; J2: 1×16 ESP32 harness; J3: 1×7 L298N logic harness; J4: servo; J5: generic 3.3 V accessory expansion; J6: buzzer connector.
- D1–D3 with R3–R5: status LEDs/resistors; Q1/R1/R2: buzzer low-side stage; C1/C2: accessory rail buffering/decoupling.

### Before manufacturing

This is a draft, not a fabricated-and-tested board release. I have not run KiCad DRC in the current environment. In KiCad:

1. Open `CyberCar_SPARK_Redline_Carrier.kicad_pro` or the `.kicad_pcb` file in PCB Editor.
2. Press **B** to refill copper zones.
3. Run **Inspect → Design Rules Checker** and resolve clearance, unconnected, silkscreen and board-outline problems.
4. Inspect both copper layers and confirm each trace meets the correct pad/net. Review the board in 3D view.
5. Verify the real purchased parts match every connector pitch, body outline, pad size, mounting hole and solder clearance.
6. Confirm C1 polarity, Q1 base/collector/emitter lead order, servo pin order and L298N enable-jumper instructions.
7. Check trace widths against your fab's copper thickness, voltage/current and temperature-rise limits.
8. With no power applied, continuity-test +5 V, GND and all signal nets. Then bring up with a current-limited supply and no motors/servo attached.
9. Only after review passes, plot Gerbers and drill files in KiCad and inspect them in an independent Gerber viewer. Gerbers are intentionally not bundled as fabrication-approved outputs.

See the full [FABRICATION_AND_REVIEW.md](https://github.com/user-attachments/files/33843515/FABRICATION_AND_REVIEW.md)
 and [README.md](https://github.com/user-attachments/files/33843531/README.md)


## 10. Firmware layout and header files

The code is split so hardware choices and policies have single sources of truth:

| File | Responsibility |
|---|---|
| `CyberCarPro_optimized.ino` | Setup, watchdog, status LED, sound and servo wrappers, car-controller behavior, main loop. |
| `config.h` | `constexpr` pins, AP credentials, command timeouts, rate limits and drive timing constants. |
| `motor_driver.h` | L298N direction signals, digital enable outputs, drive-level timings and stop/brake states. |
| `captive_portal.h` | Wi-Fi AP, DNS/captive routes, dashboard HTML/CSS/JS, request handlers and session gating. |
| `secure_backend.h` | Security response headers, bounded integer parsing, allowed-value validation, origin check, constant-time PIN compare and JSON escaping helper. |
| `face_assets.h` | JPEG payloads stored in flash (`PROGMEM`) and served to the browser without copying all images into mutable RAM buffers. |

### Why `constexpr` is used

Compile-time settings—pin numbers, timeout values, AP name, speed timing windows, and fixed UI limits—are declared `constexpr` in `config.h`. Current joystick position, session token, active command and emergency-stop state change while the program is running, so they must remain mutable runtime values.

To add a feature, place fixed hardware/settings constants in `config.h`, related reusable behavior in a `.h` module, and keep the main `.ino` focused on integration. Use header guards or `#pragma once`, keep definitions that would cause multiple-definition errors out of headers unless marked appropriately, and update README/pinout/wiring when the interface changes. Read [HEADER_FILES_GUIDE.md](HEADER_FILES_GUIDE.md) before reorganizing files.

## 11. Build and upload

### Requirements

- Arduino IDE 2.x or a compatible Arduino CLI setup.
- Espressif ESP32 board package with an Arduino-ESP32 3.x API compatible with the source.
- Board selection for LOLIN S2 Mini / ESP32-S2 matching the hardware.
- `ESP32Servo` library if the servo-related code is compiled in your local configuration.
- USB data cable, multimeter, suitable battery/buck supply, and access to a physical battery disconnect.

### Steps

1. Extract the project ZIP to a normal writable folder (not inside a ZIP preview).
2. Open `CyberCar/CyberCarPro_optimized/CyberCarPro_optimized.ino` in Arduino IDE.
3. Keep `config.h`, `motor_driver.h`, `secure_backend.h`, `face_assets.h` and `captive_portal.h` in the same sketch directory.
4. Open `config.h`; change the sample AP password and control PIN before public demonstrations. Keep credentials out of public commits.
5. Verify the board selection and USB port; compile and read all diagnostics.
6. Lift the chassis so the wheels are clear. Power the ESP32 through one verified supply path and keep your hand near the physical power switch.
7. Upload. Open Serial Monitor at **115200 baud** if logs are needed.
8. Join the AP and open the dashboard address below. Enter the configured control PIN.
9. Test STOP first, then brief direction commands with wheels raised; test the timeout and emergency latch before floor driving.

### Example local settings in this package

| Setting | Current sample value | Where to edit |
|---|---|---|
| Wi-Fi SSID | `Cyber-Car-Pro` | `config.h` → `Config::AP_SSID` |
| Wi-Fi password | `NeonDrive_2026!` | `config.h` → `Config::AP_PASSWORD` |
| Dashboard URL | `http://192.168.4.1/` | AP configuration / web routes |
| Control PIN | `583104` | `config.h` → `Config::CONTROL_PIN` |

These are sample/default values for project continuity, not unique per-device credentials. Change them before using the car around other people or sharing the firmware.

## 12. Motor control and speed behavior

The current motor driver deliberately avoids ESP32 LEDC hardware-PWM calls. It uses digital GPIO direction signals plus digital enable gating:

- **FAST:** enable pins stay HIGH during commanded movement.
- **MEDIUM:** enable pins are switched on/off in a timed window using `MEDIUM_ON_MS` and `DRIVE_WINDOW_MS`.
- **SLOW:** enable pins are switched on/off using `SLOW_ON_MS` and the same window.
- **STOP:** direction outputs are cleared and enables are disabled.
- **EMERGENCY BRAKE:** software latch switches the H-bridge inputs to the configured braking state and rejects further movement until the resume route is used.

The MEDIUM/SLOW pattern is coarse time-domain switching; it can cause jerking, noise, lower torque or inconsistent movement and it is not a precision speed controller. If smooth low-speed motion matters, use an appropriate motor driver with a PWM input or a properly engineered motor-voltage controller. Never attempt to drive motors directly from ESP32 pins.

A command watchdog/stale-command timeout stops movement when joystick updates stop arriving. The emergency brake is still a software control and is not guaranteed to stop a moving vehicle mechanically under all conditions. Add encoders/current sensing/physical power cutoff as future safety improvements.

## 13. Dashboard/API overview

The actual local API is implemented in `captive_portal.h`. The page uses same-origin requests to the local ESP32 host.

| Route | Method | Purpose | Guardrails |
|---|---|---|---|
| `/` | GET | Serves dashboard HTML | Security response headers; opening the page does not grant control. |
| `/api/login` | POST | Verifies the local control PIN and creates a session cookie | Expected origin, bounded PIN length, constant-time compare, lockout after repeated failures. |
| `/api/state` | GET | Reads current command, level, uptime, memory/status fields | Valid session and rate limit. |
| `/api/joy?x=…&y=…` | POST | Maps bounded joystick coordinates to direction/stop | Valid session, same-origin check, rate limit, x/y range validation. |
| `/api/speed?level=…` | POST | Sets `slow`, `medium`, or `fast` | Allow-listed values, session, origin check, rate limit. |
| `/api/ball?action=…` | POST | Locks or releases servo mechanism | Allow-listed action, session and origin check. |
| `/api/horn` | POST | Plays a short buzzer signal | Session and origin check. |
| `/api/brake` | POST | Latches emergency stop | Session and origin check. |
| `/api/resume` | POST | Clears the latch but leaves the car stopped until another command | Session and origin check. |
| `/assets/faces/*.jpg` | GET | Serves static image bytes from flash | Read-only asset routes; cache headers. |

The joystick is encoded as small bounded integers, not arbitrary JavaScript or executable JSON. The API uses fixed JSON response bodies for most replies; `escapeJsonString()` is a helper for any future dynamic JSON strings. Do not concatenate untrusted input into HTML/JSON, accept arbitrary filenames, or execute received strings.

## 14. Security model and limits

Security improvements are defense-in-depth for a local hobby device; they do not make the device suitable for public exposure.

- **Network boundary:** local ESP32 access point only; no cloud relay is included.
- **PIN gate:** control requires an authentication route; change the sample PIN.
- **Session:** token-based same-origin session cookie with expiry; the browser must preserve the cookie.
- **Input bounds:** joystick coordinates, speed names and accessory actions are range/allow-list checked.
- **Origin checking:** state-changing routes check the expected local origin.
- **Request rate limits:** reduce burst traffic and accidental overload.
- **Response headers:** include common browser hardening headers and a restrictive same-origin policy for the current inline UI.
- **Safe stop:** missing command refreshes trigger a stop; emergency brake is latched.

### Limitations

- Local HTTP is **not encrypted**. Do not route the portal to the public Internet or use it over an untrusted shared network.
- A six-digit PIN has limited entropy; this design is intended for a local prototype. Keep the AP private and change credentials.
- The browser/app model is not a replacement for secure boot, signed updates, physical access control, TLS, or professional penetration testing.
- The current firmware has no obstacle sensing. Add any future sensor only after validating voltage levels, protocol, pin budget, power draw, failure states and wiring.
- Do not claim “JSON injection protection” as a guarantee that all injection is impossible; the code reduces risk by using strict parsing, fixed operations and correct escaping for future dynamic fields.

More information is in [SECURE_BACKEND.md](https://github.com/user-attachments/files/33843496/SECURE_BACKEND.md)



## 15. Face images and memory efficiency

The uploaded face expressions are optimized into compact JPEG assets and embedded in `face_assets.h` in flash memory (`PROGMEM`). Read-only routes serve the JPEG byte arrays directly. The ESP32 does not decode all images into RAM, and the browser chooses the current image state.

The current state map is defined in `captive_portal.h`:

| Situation | Face asset |
|---|---|
| Idle / stopped | `idle.jpg` |
| Forward | `happy.jpg` |
| Reverse | `brake.jpg` (used as a face mood, not a motor-brake command) |
| Left turn | `dance.jpg` |
| Right turn | `drift.jpg` |
| FAST / boost | `dance2.jpg` |
| Emergency latch | `angry.jpg` |
| Error / boot fallback | `boot.jpg` |

To change the art, replace and optimize the images, update the generated byte arrays in `face_assets.h`, and keep the HTTP route names and JS map synchronized. Serve only a single current face in the DOM, choose small dimensions, avoid animated GIF/video, and use CSS transitions for motion in the browser. `PROGMEM` reduces mutable RAM usage but still consumes firmware flash space. Review [FACE_ASSETS_GUIDE.md](FACE_ASSETS_GUIDE.md) and [BUILD_AND_MEMORY_NOTES.md](BUILD_AND_MEMORY_NOTES.md).

The images were supplied in this conversation as project assets. Their ownership/redistribution rights have not been independently verified; see [LICENSE-IMAGES-AND-ASSETS.md](LICENSE-IMAGES-AND-ASSETS.md) before publishing a public repo or reusing imagery.

## 16. Testing and validation checklist

### Power and hardware
- [ ] With power disconnected, check +5 V/GND for shorts and incorrect polarity.
- [ ] Adjust the buck converter and measure 5.00 V before attaching the electronics.
- [ ] Confirm common ground between ESP32, L298N, buck, carrier and servo.
- [ ] Confirm motor battery voltage and peak current requirements match the motor and L298N module.
- [ ] Remove L298N ENA/ENB jumpers when GPIO11/GPIO12 drive the enable inputs.
- [ ] Confirm the exact L298N module's +5 V regulator jumper behavior; do not assume all boards are wired identically.
- [ ] Confirm C1 capacitor polarity, LED orientation, transistor lead order and servo lead order.
- [ ] Check the ESP32 doesn't receive two conflicting 5 V supply paths.

### Firmware/dashboard
- [ ] Compile with the intended Arduino-ESP32 core and required libraries.
- [ ] Verify the AP is visible and the phone stays on it despite the no-Internet indicator.
- [ ] Confirm the dashboard opens at `http://192.168.4.1/` and wrong PIN attempts are rejected.
- [ ] With wheels lifted, issue brief forward/reverse/left/right commands.
- [ ] Confirm release/STOP returns to stopped state.
- [ ] Engage emergency stop; confirm joystick motion remains blocked until explicit resume.
- [ ] Release joystick/stop sending updates and confirm the command timeout stops the wheels.
- [ ] Test FAST/MEDIUM/SLOW on a clear floor with supervision; note jerking and motor temperature.
- [ ] Reset power and verify the car starts in a stopped state.

### PCB
- [ ] Open the PCB in KiCad and refill zones.
- [ ] Run DRC and resolve all warnings/errors before manufacturing.
- [ ] Confirm the board outline and mounting holes fit the intended chassis.
- [ ] Verify the bought header/terminal block footprint and pin order matches the board.
- [ ] Inspect the copper, clearances, silkscreen, solder mask and GND zone connections.
- [ ] Continuity-test every harness pin; check against `CONNECTOR_PINOUT.csv`.
- [ ] Generate Gerbers only after design verification and inspect them in an independent viewer.

## 17. Troubleshooting

| Problem | First checks |
|---|---|
| Wi-Fi network missing | USB power, board boot, serial logs, AP configuration and firmware upload status. |
| Phone says “no Internet” | Normal for a standalone AP. Stay connected and manually open `http://192.168.4.1/`. |
| Portal opens but joystick does nothing | Login session, phone Wi-Fi, AP host address, browser request errors, command rate limits, Serial Monitor output, and motor supply. |
| No motor movement | Battery, L298N motor supply, common ground, enable jumpers, IN/EN wiring, motor outputs and motor terminal fit. |
| One motor runs backward | Swap that motor's two OUT wires or toggle the matching inversion setting after powering down. |
| Medium/slow stutters | Expected from coarse digital ON/OFF bursts; adjust timing cautiously or move to a PWM-capable driver for smooth speed. |
| ESP32 resets when servo moves | Buck peak current, ground/wire resistance, servo jam/stall, bulk capacitance and dual-supply backfeeding. |
| Buzzer silent | Confirm passive buzzer type, Q1 lead order, R1/R2 wiring, supply polarity and GPIO18 assignment. |
| Face picture not showing | Check route name and JavaScript asset map; confirm face arrays/sizes in `face_assets.h`; avoid mismatched file sizes. |
| PCB DRC complains | Refill zones, verify footprints/nets and review `FABRICATION_AND_REVIEW.md`. Do not ignore unconnected-pad or clearance failures. |

## 18. How to make the robot better

Recommended roadmap, from most fundamental to more advanced:

1. **Power integrity:** use protected battery wiring, an accessible disconnect, stable regulated rails and measured motor current.
2. **Mechanical reliability:** secure the battery, strain-relieve motor wires, isolate sharp board corners, and check wheel/motor alignment.
3. **Closed-loop movement:** add wheel encoders and measure speed instead of assuming the commanded power equals a known speed.
4. **Replaceable sensing:** choose a future distance/line/IMU/camera module based on the experiment, then check logic voltage, current, mounting, sampling rate and fail-safe behavior before integration.
5. **Telemetry:** log commanded direction, measured wheel speed, battery voltage (with a properly designed divider/ADC calibration if implemented), resets and command dropouts.
6. **Behavior state machine:** make IDLE, MANUAL, AUTONOMOUS, BRAKE and FAULT explicit states; define which transitions are allowed and what happens after a fault.
7. **AI in a measurable way:** define a task, collect labelled/telemetry data, build a baseline, measure success and failure rates, and only then deploy a model/policy. Do not label UI mock panels as working AI.
8. **PCB Rev B:** after Rev A bench testing, add correctly sized protection, keyed connectors, clear test points, mounting fit checks and (if justified) voltage/current measurement.
9. **Software quality:** use version control, issue tracking, code review for motor/security changes, repeatable compile settings and a short test log for every release.

## 19. Contributing and issue reports

Before sending a patch, include: goal and scope, changed files, compile board/core versions, wiring changes, test conditions, expected behavior, actual behavior and a screenshot/log where useful. Never include personal Wi-Fi passwords or private API secrets in a public issue. Safety-critical changes—motor direction, enable logic, brake, power path or authentication—should receive a second-person review.

## 20. License scope and third-party material

This package intentionally separates software from hardware licensing:

- `LICENSE`: complete Apache License 2.0 text for software/documentation portions that the relevant contributors have authority to license.
- `LICENSE-HARDWARE`: complete unmodified CERN Open Hardware Licence Version 2 — Strongly Reciprocal (`CERN-OHL-S-2.0`) text for the covered carrier PCB source, subject to the scope note in `LICENSE-HARDWARE-NOTICE.md`.
- `LICENSE-OVERVIEW.md` and `NOTICE.md`: scope, attribution and practical redistribution notes.
- `LICENSE-IMAGES-AND-ASSETS.md`: source/face images, team likenesses, logos, icons, fonts and other artwork are not automatically relicensed by the software or hardware licence.
- Third-party libraries and external module designs stay under their own terms.

A license file cannot grant rights the contributors do not own. Before public release or hardware resale, verify authorship, obtain permission from every applicable contributor/rights holder, record a stable source location for the covered PCB design, and check all included photos and artwork. This repository is an educational prototype, not a legal opinion or a safety certification.

## 21. Quick links

- [SECURE_BACKEND.md](https://github.com/user-attachments/files/33843100/SECURE_BACKEND.md)
- [WIRING.md](https://github.com/user-attachments/files/33843136/WIRING.md)
- [HEADER_FILES_GUIDE.md](https://github.com/user-attachments/files/33843170/HEADER_FILES_GUIDE.md)
- [FACE_ASSETS_GUIDE.md](https://github.com/user-attachments/files/33843266/FACE_ASSETS_GUIDE.md)
- [BUILD_AND_MEMORY_NOTES.md](https://github.com/user-attachments/files/33843323/BUILD_AND_MEMORY_NOTES.md)
- [LICENSE-IMAGES-AND-ASSETS.md](https://github.com/user-attachments/files/33843360/LICENSE-IMAGES-AND-ASSETS.md)
- [FABRICATION_AND_REVIEW.md](https://github.com/user-attachments/files/33843394/FABRICATION_AND_REVIEW.md)
- [README.md](https://github.com/user-attachments/files/33843408/README.md)

