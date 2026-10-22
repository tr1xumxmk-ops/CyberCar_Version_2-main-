# Secure backend notes — CyberCar Spark

The ESP32's `WebServer` is the backend for the private local captive portal. `secure_backend.h` is included by `captive_portal.h` and is actually used for security headers, strict numeric parsing, origin checks, constant-time comparisons, value allowlists, and JSON escaping.

## Request flow
1. Phone joins the car's `Cyber-Car-Pro` Wi-Fi network.
2. The home page displays a PIN prompt. A successful POST to `/api/login` creates a random session token and returns it in an HttpOnly, SameSite=Strict cookie.
3. Movement/action APIs require that cookie and the `http://192.168.4.1` Origin. API calls have fixed route names and fixed accepted values; movement coordinates are bounded integers.
4. Firmware enforces rate limits and stops movement if joystick keepalives disappear. Emergency stop is latched until explicitly resumed.

## JSON injection
The API does not accept executable JSON and never evaluates input. Current telemetry JSON is assembled from fixed internal enum labels, booleans, and integers. `SecureBackend::escapeJsonString` is provided for future dynamic JSON string values; never concatenate untrusted text directly into a JSON document. Parsing an arbitrary JSON request body is unnecessary for the current small API and is intentionally avoided.

## Set your own PIN
Change `Config::CONTROL_PIN` in `config.h` before uploading. The default in this sample is `583104`. Five failed attempts cause a 30-second lockout. The AP password should also be changed before public demonstrations.

## Security boundary
The ESP32 captive portal uses HTTP and is designed for a direct, private Wi-Fi link. It is not a public-Internet backend and should not be port-forwarded. For remote Internet control, use a broker/relay with TLS and identity-based access control instead of opening this HTTP port.
