# SPARK CAR — QR Quick Connect

This small Python tool makes it easier to connect a phone to the SPARK CAR Wi-Fi hotspot and open the car's web dashboard.

## What is different from the old method?

| Old method | QR helper |
|---|---|
| Open the phone's Wi-Fi settings manually. | Scan the **Wi-Fi QR** and approve the connection prompt. |
| Find the car's network and type its password. | The QR contains the network name and password, so you usually do not need to type them. |
| Open the browser and type `http://192.168.4.1/`. | After connecting, scan the **Dashboard QR** to open the portal address. |
| Share connection instructions separately. | Print or share one card containing both QR codes and the instructions. |

## What the Python script creates

- `spark_car_wifi_qr.png` — QR code for joining the car's Wi-Fi.
- `spark_car_dashboard_qr.png` — QR code for opening the web dashboard.
- `spark_car_quick_connect_card.png` — printable card with both codes.
- `connection_details.txt` — connection settings and instructions.

## Default connection settings

- Wi-Fi name: `Cyber-Car-Pro`
- Wi-Fi password: `NeonDrive_2026!`
- Dashboard: `http://192.168.4.1/`

These defaults match the original CyberCar configuration used when this script was prepared. If the firmware uses a different SSID or password, regenerate the codes with matching values.

## Install and run

Install Python 3, then run in this folder:

```bash
python -m pip install "qrcode[pil]" Pillow
python spark_car_qr.py
```

To use a different Wi-Fi password:

```bash
python spark_car_qr.py --password 12345678
```

To change both the network name and password:

```bash
python spark_car_qr.py --ssid Spark-Car --password 12345678
```

The images will be saved in `spark_car_qr_output/`.

## Important limitation

The QR helper does **not** change the ESP32 firmware and does not control the car by itself. It only makes connecting easier. Scan the Wi-Fi QR first, approve the phone's connection prompt, then scan the dashboard QR if the captive portal does not open automatically.

A standard Wi-Fi QR cannot reliably make every Android and iPhone both join the network and open the website with one scan, so this project uses two QR codes. Keep the combined card private because the Wi-Fi QR contains the network password.
