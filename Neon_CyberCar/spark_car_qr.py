#!/usr/bin/env python3
"""Generate Wi-Fi and dashboard QR codes for the CyberCar / SPARK CAR ESP32 portal.

Install: python -m pip install -r requirements.txt
Run:     python spark_car_qr.py

Note: Wi-Fi QR standards join a network; URL QR codes open a website. A single QR
code cannot reliably perform both actions across Android and iOS, so this script
creates two scan-ready QR codes and a printable card containing both.
"""
from __future__ import annotations

import argparse
import sys
import webbrowser
from pathlib import Path
from typing import Optional

try:
    import qrcode
    from qrcode.constants import ERROR_CORRECT_M
    from PIL import Image, ImageDraw, ImageFont
except ImportError as exc:
    print("Missing dependency. Install with: python -m pip install -r requirements.txt", file=sys.stderr)
    raise SystemExit(2) from exc

# Keep these in sync with Config::AP_SSID and Config::AP_PASSWORD in config.h.
DEFAULT_SSID = "Cyber-Car-Pro"
DEFAULT_PASSWORD = "NeonDrive_2026!"
DEFAULT_URL = "http://192.168.4.1/"
DEFAULT_OUTPUT = "spark_car_qr_output"

NAVY = (7, 12, 30)
PANEL = (15, 24, 48)
CYAN = (0, 225, 255)
MAGENTA = (255, 50, 220)
WHITE = (245, 249, 255)
MUTED = (155, 177, 205)
BLACK = (0, 0, 0)


def wifi_escape(value: str) -> str:
    """Escape special characters per common WIFI: QR payload conventions."""
    out = value
    for char in ("\\", ";", ",", '"', ":"):
        out = out.replace(char, "\\" + char)
    return out


def qr_image(payload: str, box_size: int = 12) -> Image.Image:
    qr = qrcode.QRCode(
        version=None,
        error_correction=ERROR_CORRECT_M,
        box_size=box_size,
        border=4,
    )
    qr.add_data(payload)
    qr.make(fit=True)
    return qr.make_image(fill_color=BLACK, back_color="white").convert("RGB")


def load_font(size: int, bold: bool = False) -> ImageFont.ImageFont:
    candidates = []
    if sys.platform.startswith("win"):
        candidates += [r"C:\Windows\Fonts\arialbd.ttf" if bold else r"C:\Windows\Fonts\arial.ttf"]
    candidates += [
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf" if bold else "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationSans-Bold.ttf" if bold else "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf",
    ]
    for path in candidates:
        try:
            return ImageFont.truetype(path, size=size)
        except OSError:
            pass
    return ImageFont.load_default()


def fit_font(draw: ImageDraw.ImageDraw, text: str, max_width: int, start: int, min_size: int = 18, bold: bool = False) -> ImageFont.ImageFont:
    for size in range(start, min_size - 1, -1):
        font = load_font(size, bold)
        box = draw.textbbox((0, 0), text, font=font)
        if box[2] - box[0] <= max_width:
            return font
    return load_font(min_size, bold)


def draw_centered(draw: ImageDraw.ImageDraw, x_center: int, y: int, text: str, font: ImageFont.ImageFont, fill: tuple[int, int, int]) -> None:
    box = draw.textbbox((0, 0), text, font=font)
    width = box[2] - box[0]
    draw.text((x_center - width // 2, y), text, font=font, fill=fill)


def make_card(wifi_qr: Image.Image, portal_qr: Image.Image, ssid: str, password: str, url: str, output_path: Path, print_password: bool = True) -> None:
    """Build an A4-ish landscape card with both QR codes on clean white panels."""
    width, height = 1800, 1120
    image = Image.new("RGB", (width, height), NAVY)
    draw = ImageDraw.Draw(image)

    # Outer neon frame and header.
    draw.rounded_rectangle((18, 18, width - 18, height - 18), radius=34, outline=CYAN, width=4)
    draw.line((55, 145, width - 55, 145), fill=MAGENTA, width=3)
    draw.text((65, 40), "SPARK TEAM  //  CYBERCAR", font=load_font(46, True), fill=CYAN)
    draw.text((67, 98), "QUICK CONNECT CARD  •  ESP32-S2 LOCAL HOTSPOT", font=load_font(23, True), fill=WHITE)

    # QR panels.
    panel_top, panel_bottom = 180, 885
    left_box = (55, panel_top, 855, panel_bottom)
    right_box = (945, panel_top, 1745, panel_bottom)
    for box, accent in ((left_box, CYAN), (right_box, MAGENTA)):
        draw.rounded_rectangle(box, radius=28, fill=PANEL, outline=accent, width=3)

    draw_centered(draw, 455, 205, "1  •  JOIN THE CAR WI-FI", load_font(30, True), CYAN)
    draw_centered(draw, 1345, 205, "2  •  OPEN THE DASHBOARD", load_font(30, True), MAGENTA)

    # Scale QR codes into equal white squares with generous quiet zone.
    qr_size = 500
    def contain(img: Image.Image, size: int) -> Image.Image:
        img = img.copy()
        img.thumbnail((size, size), Image.Resampling.NEAREST)
        canvas = Image.new("RGB", (size, size), "white")
        canvas.paste(img, ((size - img.width) // 2, (size - img.height) // 2))
        return canvas

    q1 = contain(wifi_qr, qr_size)
    q2 = contain(portal_qr, qr_size)
    image.paste(q1, (455 - qr_size // 2, 265))
    image.paste(q2, (1345 - qr_size // 2, 265))

    draw_centered(draw, 455, 785, f"Wi-Fi: {ssid}", fit_font(draw, f"Wi-Fi: {ssid}", 730, 28, 17, True), WHITE)
    if print_password:
        pass_text = f"Password: {password}"
    else:
        pass_text = "Password: (use the one configured in firmware)"
    draw_centered(draw, 455, 825, pass_text, fit_font(draw, pass_text, 730, 22, 15, False), MUTED)
    draw_centered(draw, 1345, 785, url, fit_font(draw, url, 730, 26, 16, True), WHITE)
    draw_centered(draw, 1345, 825, "Scan after your phone joins the Wi-Fi", load_font(20), MUTED)

    draw.text((65, 920), "Tip: after connecting, your phone may open the captive portal automatically.", font=load_font(23), fill=WHITE)
    draw.text((65, 963), "If not, scan QR #2 or enter the local URL shown above in your browser.", font=load_font(23), fill=MUTED)
    draw.text((65, 1030), "Keep this card private: the Wi-Fi QR contains the network password.", font=load_font(19, True), fill=(255, 105, 105))
    image.save(output_path, quality=95)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate Wi-Fi and portal QR codes for SPARK CAR.")
    parser.add_argument("--ssid", default=DEFAULT_SSID, help=f"Wi-Fi SSID (default: {DEFAULT_SSID})")
    parser.add_argument("--password", default=DEFAULT_PASSWORD, help="Wi-Fi password (default matches current original project config)")
    parser.add_argument("--url", default=DEFAULT_URL, help=f"Captive portal URL (default: {DEFAULT_URL})")
    parser.add_argument("--output", default=DEFAULT_OUTPUT, help=f"Output directory (default: {DEFAULT_OUTPUT})")
    parser.add_argument("--no-print-password", action="store_true", help="Do not print the password as readable text on the combined card")
    parser.add_argument("--open-dashboard", action="store_true", help="Open the dashboard URL in this computer's default browser after generating files")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    ssid = args.ssid.strip()
    password = args.password
    url = args.url.strip()
    if not ssid:
        print("Error: SSID cannot be empty.", file=sys.stderr)
        return 2
    if len(ssid.encode("utf-8")) > 32:
        print("Error: Wi-Fi SSID must be at most 32 bytes.", file=sys.stderr)
        return 2
    if len(password) < 8:
        print("Error: WPA/WPA2 Wi-Fi password must be at least 8 characters.", file=sys.stderr)
        return 2
    if not url.lower().startswith(("http://", "https://")):
        print("Error: URL must begin with http:// or https://", file=sys.stderr)
        return 2

    out_dir = Path(args.output).expanduser().resolve()
    out_dir.mkdir(parents=True, exist_ok=True)
    wifi_payload = f"WIFI:T:WPA;S:{wifi_escape(ssid)};P:{wifi_escape(password)};;"

    wifi = qr_image(wifi_payload, box_size=14)
    portal = qr_image(url, box_size=14)
    wifi_path = out_dir / "spark_car_wifi_qr.png"
    portal_path = out_dir / "spark_car_dashboard_qr.png"
    card_path = out_dir / "spark_car_quick_connect_card.png"
    txt_path = out_dir / "connection_details.txt"

    wifi.save(wifi_path)
    portal.save(portal_path)
    make_card(wifi, portal, ssid, password, url, card_path, print_password=not args.no_print_password)
    txt_path.write_text(
        "SPARK CAR connection details\n"
        f"Wi-Fi name: {ssid}\n"
        f"Wi-Fi password: {password}\n"
        f"Dashboard URL: {url}\n\n"
        "Workflow: scan the Wi-Fi QR and accept the join prompt. Then scan the dashboard QR, "
        "or open the URL manually. Some phones open captive portals automatically; this is OS-dependent.\n",
        encoding="utf-8",
    )

    print("Created QR files:")
    print(f"  Wi-Fi QR:       {wifi_path}")
    print(f"  Dashboard QR:   {portal_path}")
    print(f"  Combined card:  {card_path}")
    print(f"  Details:        {txt_path}")
    print("\nImportant: a Wi-Fi QR joins the network; a URL QR opens the webpage.")
    print("A single QR cannot reliably perform both actions across Android and iOS.")
    if args.open_dashboard:
        print("Opening the portal URL in this computer's browser. This does not connect the computer to the car Wi-Fi.")
        webbrowser.open(url, new=2)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
