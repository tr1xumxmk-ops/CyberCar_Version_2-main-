# SPARK REDLINE face assets

All 11 expressions from the supplied `pics.zip` are used. The firmware serves compact JPEGs from flash using `send_P()`, so there is no LittleFS upload step and the browser only requests the face needed for the current state.

## Mapping

- `IDLE.png` -> `/assets/faces/idle.jpg` -> stopped
- `HAPPY.png` -> `/assets/faces/happy.jpg` -> forward
- `BRAKE.png` -> `/assets/faces/brake.jpg` -> reverse
- `DANCE.png` -> `/assets/faces/dance.jpg` -> left
- `DRIFT.png` -> `/assets/faces/drift.jpg` -> right
- `DANCE_2.png` -> `/assets/faces/dance2.jpg` -> fast-forward expression
- `ANGRY.png` -> `/assets/faces/angry.jpg` -> emergency stop
- `boot.png` -> `/assets/faces/boot.jpg` -> boot/login graphic
- `MANUAL.png` -> `/assets/faces/manual.jpg` -> active manual-control profile card
- `AUTO.png` -> `/assets/faces/auto.jpg` -> planned-mode visual only
- `QLEARNING.png` -> `/assets/faces/qlearning.jpg` -> planned-mode visual only

The raster versions were reduced to 240 x 160 and recolored in a black / hazard-yellow / red palette. Source originals are retained in `original_uploaded_face_images/`. Replace one of the optimized JPEGs and regenerate `face_assets.h` if you want to change artwork; both the header and JPEG source folder are included.

## Memory notes

The JPEGs are stored as byte arrays in `face_assets.h` with `PROGMEM`. This increases firmware flash usage but does not reserve one full image buffer in the ESP32 heap. The WebServer sends the image bytes directly from flash. The browser sets the image source only when the state changes and can cache the static asset.

## Limits

AUTO and Q-LEARNING tiles are marked as visual concepts only. The present firmware implements manual joystick control; these images do not turn on autonomous navigation or a trained policy.
