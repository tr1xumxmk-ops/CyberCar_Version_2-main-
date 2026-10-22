# Build, image, and memory notes

## Face image pipeline
The uploaded `pics.zip` contains 11 PNG state illustrations. The build process converts them to 240x160 JPEGs in a black/red/yellow heat palette and keeps the originals for editability. `face_assets.h` embeds the compact JPEG bytes into program flash using `PROGMEM`. This does consume firmware flash, but not a persistent RAM buffer. The `send_P(..., size)` overload sends binary image data without calling `String` on it.

## To replace an image
1. Edit/replace the corresponding source in `original_uploaded_face_images/`.
2. Resize to 240x160, recolor to black/red/yellow, and compress as JPEG quality around 75–80.
3. Replace the matching `.jpg` in `CyberCar/CyberCarPro_optimized/web_assets/faces/`.
4. Regenerate the byte array in `face_assets.h` (or ask for another pack generation).

## Features honestly represented
- Live: MANUAL joystick, FAST/MEDIUM/SLOW digital drive profiles, emergency stop/resume, servo/horn controls, session PIN gate and timeout.
- Visual-only roadmap: AUTO and Q-LEARNING profile cards. They are not active navigation modes in the current source.
- The firmware contains no distance-sensor driver or sensor GPIO assignments.
