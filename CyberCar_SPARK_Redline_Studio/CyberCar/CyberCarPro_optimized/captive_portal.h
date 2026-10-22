// CYBER CAR SPARK REDLINE — offline captive portal, authenticated local API, animated HUD
#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <esp_system.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "secure_backend.h"
#include "face_assets.h"

static constexpr char SESSION_COOKIE_NAME[] = "cyber_car_session";

class PortalServer {
 public:
  explicit PortalServer(CarController& car) : server_(80), car_(car) {}

  void begin() {
    const IPAddress ip(Config::IP_OCTET_1, Config::IP_OCTET_2,
                       Config::IP_OCTET_3, Config::IP_OCTET_4);
    const IPAddress mask(255, 255, 255, 0);
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(ip, ip, mask);
    const bool apStarted = WiFi.softAP(Config::AP_SSID, Config::AP_PASSWORD,
                                       Config::AP_CHANNEL, false, Config::AP_MAX_CLIENTS);
    if (!apStarted) Serial.println(F("[ERROR] Wi-Fi access point failed to start."));
    generateToken();

    static const char* kHeaders[] = {"Cookie", "Origin"};
    server_.collectHeaders(kHeaders, 2);
    routes();
    server_.begin();
    dns_.start(Config::DNS_PORT, "*", ip);

    Serial.println(F("=============================================="));
    Serial.println(F(" CYBER CAR SPARK REDLINE — LOCAL CONTROL READY"));
    Serial.printf(" SSID: %s\n", Config::AP_SSID);
    Serial.println(F(" Dashboard: http://192.168.4.1/"));
    Serial.println(F(" Commands: digital GPIO control, no LEDC motor PWM"));
    Serial.println(F(" Do not expose this HTTP portal to the public Internet."));
    Serial.println(F("=============================================="));
  }

  void update() {
    dns_.processNextRequest();
    server_.handleClient();
  }

 private:
  void generateToken() {
    static constexpr char kHex[] = "0123456789abcdef";
    uint32_t word = 0;
    for (uint8_t i = 0; i < Config::TOKEN_HEX_LENGTH; ++i) {
      if ((i % 8U) == 0U) word = esp_random();
      sessionToken_[i] = kHex[word & 0x0FU];
      word >>= 4U;
    }
    sessionToken_[Config::TOKEN_HEX_LENGTH] = '\0';
    sessionCreatedAt_ = millis();
  }

  bool tokenMatches(const String& token) const {
    return SecureBackend::constantTimeEquals(token.c_str(), token.length(),
                                             sessionToken_, Config::TOKEN_HEX_LENGTH);
  }

  bool hasSessionCookie() const {
    const String cookie = server_.header("Cookie");
    int start = 0;
    while (start < cookie.length()) {
      int end = cookie.indexOf(';', start);
      if (end < 0) end = cookie.length();
      String item = cookie.substring(start, end);
      item.trim();
      const int split = item.indexOf('=');
      if (split > 0 && item.substring(0, split).equals(SESSION_COOKIE_NAME)) {
        return tokenMatches(item.substring(split + 1));
      }
      start = end + 1;
    }
    return false;
  }

  bool authorized() const {
    constexpr uint32_t kSessionMaxAgeMs = Config::SESSION_MAX_AGE_SECONDS * 1000UL;
    return !elapsed(millis(), sessionCreatedAt_, kSessionMaxAgeMs) && hasSessionCookie();
  }

  void secureHeaders() { SecureBackend::applySecurityHeaders(server_); }

  void json(const int status, const char* body) {
    // Caller passes constant JSON or JSON constructed only from validated enum/numeric values.
    SecureBackend::sendJson(server_, status, body);
  }

  bool requireAuth() {
    if (authorized()) return true;
    json(403, "{\"ok\":false,\"error\":\"open_dashboard_first\"}");
    return false;
  }

  bool requireSameOrigin() {
    // All state-changing browser requests must originate from this local dashboard.
    const String origin = server_.header("Origin");
    if (SecureBackend::trustedOrigin(origin)) return true;
    json(403, "{\"ok\":false,\"error\":\"origin_rejected\"}");
    return false;
  }

  bool rateLimited(uint32_t& previous, const uint32_t intervalMs) {
    const uint32_t now = millis();
    if (previous != 0U && !elapsed(now, previous, intervalMs)) return true;
    previous = now;
    return false;
  }

  bool parseIntArg(const char* name, const int minValue, const int maxValue, int& result) {
    return SecureBackend::parseBoundedIntArg(server_, name, minValue, maxValue, result);
  }

  void sendFace(const uint8_t* imageData, const size_t imageSize) {
    // Serve compressed bytes directly from flash; do not allocate a large String or RAM buffer.
    server_.sendHeader("X-Content-Type-Options", "nosniff");
    server_.sendHeader("Cache-Control", "public, max-age=86400");
    server_.send_P(200, "image/jpeg", reinterpret_cast<PGM_P>(imageData), imageSize);
  }

  void routes() {
    server_.on("/", HTTP_GET, [this]() { index(); });
    // Static face art lives in flash (PROGMEM). These GETs are read-only and need no session.
    server_.on("/assets/faces/angry.jpg", HTTP_GET, [this]() { sendFace(FaceAssets::kAngry, FaceAssets::kAngrySize); });
    server_.on("/assets/faces/auto.jpg", HTTP_GET, [this]() { sendFace(FaceAssets::kAuto, FaceAssets::kAutoSize); });
    server_.on("/assets/faces/boot.jpg", HTTP_GET, [this]() { sendFace(FaceAssets::kBoot, FaceAssets::kBootSize); });
    server_.on("/assets/faces/brake.jpg", HTTP_GET, [this]() { sendFace(FaceAssets::kBrake, FaceAssets::kBrakeSize); });
    server_.on("/assets/faces/dance.jpg", HTTP_GET, [this]() { sendFace(FaceAssets::kDance, FaceAssets::kDanceSize); });
    server_.on("/assets/faces/dance2.jpg", HTTP_GET, [this]() { sendFace(FaceAssets::kDance2, FaceAssets::kDance2Size); });
    server_.on("/assets/faces/drift.jpg", HTTP_GET, [this]() { sendFace(FaceAssets::kDrift, FaceAssets::kDriftSize); });
    server_.on("/assets/faces/happy.jpg", HTTP_GET, [this]() { sendFace(FaceAssets::kHappy, FaceAssets::kHappySize); });
    server_.on("/assets/faces/idle.jpg", HTTP_GET, [this]() { sendFace(FaceAssets::kIdle, FaceAssets::kIdleSize); });
    server_.on("/assets/faces/manual.jpg", HTTP_GET, [this]() { sendFace(FaceAssets::kManual, FaceAssets::kManualSize); });
    server_.on("/assets/faces/qlearning.jpg", HTTP_GET, [this]() { sendFace(FaceAssets::kQlearning, FaceAssets::kQlearningSize); });
    server_.on("/api/login", HTTP_POST, [this]() { login(); });
    server_.on("/api/state", HTTP_GET, [this]() {
      if (!requireAuth()) return;
      if (rateLimited(lastStateAt_, Config::STATE_RATE_LIMIT_MS)) return json(429, "{\"ok\":false,\"error\":\"rate_limited\"}");
      state();
    });
    server_.on("/api/joy", HTTP_POST, [this]() {
      if (!requireAuth() || !requireSameOrigin()) return;
      if (rateLimited(lastJoyAt_, Config::JOYSTICK_RATE_LIMIT_MS)) return json(429, "{\"ok\":false,\"error\":\"rate_limited\"}");
      joystick();
    });
    server_.on("/api/speed", HTTP_POST, [this]() {
      if (!requireAuth() || !requireSameOrigin()) return;
      if (rateLimited(lastControlAt_, Config::CONTROL_RATE_LIMIT_MS)) return json(429, "{\"ok\":false,\"error\":\"rate_limited\"}");
      speed();
    });
    server_.on("/api/ball", HTTP_POST, [this]() {
      if (!requireAuth() || !requireSameOrigin()) return;
      if (rateLimited(lastControlAt_, Config::CONTROL_RATE_LIMIT_MS)) return json(429, "{\"ok\":false,\"error\":\"rate_limited\"}");
      ball();
    });
    server_.on("/api/horn", HTTP_POST, [this]() {
      if (!requireAuth() || !requireSameOrigin()) return;
      if (rateLimited(lastControlAt_, Config::CONTROL_RATE_LIMIT_MS)) return json(429, "{\"ok\":false,\"error\":\"rate_limited\"}");
      car_.horn(); json(200, "{\"ok\":true}");
    });
    server_.on("/api/brake", HTTP_POST, [this]() {
      if (!requireAuth() || !requireSameOrigin()) return;
      car_.emergencyStop(millis()); json(200, "{\"ok\":true,\"estop\":true}");
    });
    server_.on("/api/resume", HTTP_POST, [this]() {
      if (!requireAuth() || !requireSameOrigin()) return;
      car_.resume(millis()); json(200, "{\"ok\":true,\"estop\":false}");
    });

    server_.on("/hotspot-detect.html", HTTP_GET, [this]() { redirectToPortal(); });
    server_.on("/generate_204", HTTP_GET, [this]() { redirectToPortal(); });
    server_.on("/connecttest.txt", HTTP_GET, [this]() { redirectToPortal(); });
    server_.on("/ncsi.txt", HTTP_GET, [this]() { redirectToPortal(); });
    server_.onNotFound([this]() { index(); });
  }

  bool loginLockoutActive(const uint32_t now) const {
    return loginBlockedUntil_ != 0U && static_cast<int32_t>(loginBlockedUntil_ - now) > 0;
  }

  void login() {
    if (!requireSameOrigin()) return;
    const uint32_t now = millis();
    if (loginLockoutActive(now)) return json(429, "{\"ok\":false,\"error\":\"login_locked\"}");
    if (!server_.hasArg("pin")) return json(400, "{\"ok\":false,\"error\":\"missing_pin\"}");

    const String pin = server_.arg("pin");
    const size_t expectedLength = strlen(Config::CONTROL_PIN);
    const bool valid = pin.length() <= 16U &&
      SecureBackend::constantTimeEquals(pin.c_str(), pin.length(), Config::CONTROL_PIN, expectedLength);
    if (!valid) {
      ++loginFailures_;
      if (loginFailures_ >= Config::MAX_LOGIN_FAILURES) {
        loginFailures_ = 0;
        loginBlockedUntil_ = now + Config::LOGIN_LOCKOUT_MS;
        return json(429, "{\"ok\":false,\"error\":\"login_locked\"}");
      }
      return json(401, "{\"ok\":false,\"error\":\"invalid_pin\"}");
    }

    loginFailures_ = 0;
    loginBlockedUntil_ = 0;
    generateToken(); // rotate session after successful authentication
    char cookie[128];
    snprintf(cookie, sizeof(cookie), "%s=%s; Path=/; Max-Age=%lu; HttpOnly; SameSite=Strict",
             SESSION_COOKIE_NAME, sessionToken_,
             static_cast<unsigned long>(Config::SESSION_MAX_AGE_SECONDS));
    server_.sendHeader("Set-Cookie", cookie);
    json(200, "{\"ok\":true,\"authenticated\":true}");
  }

  void redirectToPortal() {
    secureHeaders();
    server_.sendHeader("Location", "http://192.168.4.1/");
    server_.send(302, "text/plain; charset=utf-8", "");
  }

  void index() {
    secureHeaders();
    // Merely opening the portal does NOT grant control. The user must pass the PIN gate.
    server_.send_P(200, "text/html; charset=utf-8", HTML);
  }

  void joystick() {
    int x = 0, y = 0;
    if (!parseIntArg("x", -100, 100, x) || !parseIntArg("y", -100, 100, y)) {
      return json(400, "{\"ok\":false,\"error\":\"invalid_joystick\"}");
    }
    char command = 'S';
    if (abs(x) > abs(y)) {
      if (x > 22) command = 'R';
      else if (x < -22) command = 'L';
    } else {
      if (y > 22) command = 'F';
      else if (y < -22) command = 'B';
    }
    if (!car_.handleJoystick(command, millis())) {
      return json(423, "{\"ok\":false,\"error\":\"emergency_stop_latched\"}");
    }
    json(200, "{\"ok\":true}");
  }

  void speed() {
    if (!server_.hasArg("level")) return json(400, "{\"ok\":false,\"error\":\"missing_level\"}");
    const String level = server_.arg("level");
    if (SecureBackend::allowedValue(level, "slow", "medium", "fast")) {
      if (level == "slow") car_.setDriveLevel(DriveLevel::Slow);
      else if (level == "medium") car_.setDriveLevel(DriveLevel::Medium);
      else car_.setDriveLevel(DriveLevel::Fast);
    } else return json(400, "{\"ok\":false,\"error\":\"invalid_level\"}");
    json(200, "{\"ok\":true}");
  }

  void ball() {
    if (!server_.hasArg("action")) return json(400, "{\"ok\":false,\"error\":\"missing_action\"}");
    const String action = server_.arg("action");
    if (SecureBackend::allowedValue(action, "release", "lock")) {
      if (action == "release") car_.releaseBall(); else car_.lockBall();
    } else return json(400, "{\"ok\":false,\"error\":\"invalid_action\"}");
    json(200, "{\"ok\":true}");
  }

  void state() {
    char response[256];
    snprintf(response, sizeof(response),
      "{\"ok\":true,\"command\":\"%s\",\"level\":\"%s\",\"estop\":%s,"
      "\"ball\":\"%s\",\"servoAttached\":%s,\"heap\":%lu,\"uptime\":%lu}",
      MotorDriver::commandName(car_.motor().activeCommand()),
      MotorDriver::levelName(car_.motor().level()),
      car_.emergencyLatched() ? "true" : "false",
      car_.ball().locked() ? "locked" : "released",
      car_.ball().attached() ? "true" : "false",
      static_cast<unsigned long>(ESP.getFreeHeap()),
      static_cast<unsigned long>(car_.uptimeSeconds()));
    json(200, response);
  }

  DNSServer dns_;
  WebServer server_;
  CarController& car_;
  char sessionToken_[Config::TOKEN_HEX_LENGTH + 1] = {};
  uint32_t lastJoyAt_ = 0;
  uint32_t lastControlAt_ = 0;
  uint32_t lastStateAt_ = 0;
  uint32_t loginBlockedUntil_ = 0;
  uint32_t sessionCreatedAt_ = 0;
  uint8_t loginFailures_ = 0;

  static const char HTML[] PROGMEM;
};

const char PortalServer::HTML[] PROGMEM = R"CYBERHTML(
<!doctype html>
<html lang="en" dir="ltr" id="rootHtml">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
<meta name="theme-color" content="#080706">
<title>CYBER CAR / SPARK REDLINE CONTROL</title>
<style>
:root{--bg:#050505;--panel:rgba(20,15,8,.90);--line:rgba(255,205,0,.26);--cyan:#ffd000;--pink:#ff3030;--lime:#ffd000;--amber:#ffb000;--red:#ff3030;--text:#fff7df;--muted:#b2a17d;--radius:18px;--shadow:0 12px 40px rgba(0,0,0,.55)}
*{box-sizing:border-box;margin:0;padding:0;-webkit-tap-highlight-color:transparent}html{scroll-behavior:smooth}body{min-height:100vh;background:var(--bg);color:var(--text);font-family:system-ui,-apple-system,"Segoe UI",sans-serif;overflow-x:hidden}
body:before{content:"";position:fixed;inset:0;z-index:-2;pointer-events:none;background:radial-gradient(ellipse at 15% 5%,rgba(255,205,0,.16),transparent 35%),radial-gradient(ellipse at 90% 25%,rgba(255,48,48,.14),transparent 35%),linear-gradient(rgba(255,205,0,.035) 1px,transparent 1px),linear-gradient(90deg,rgba(255,205,0,.035) 1px,transparent 1px);background-size:auto,auto,32px 32px,32px 32px;animation:gridDrift 18s linear infinite}
body:after{content:"";position:fixed;inset:0;pointer-events:none;z-index:10;background:repeating-linear-gradient(0deg,transparent 0 3px,rgba(255,255,255,.018) 4px),linear-gradient(115deg,transparent 0 48%,rgba(255,205,0,.035) 50%,transparent 52%);mix-blend-mode:screen}
@keyframes gridDrift{to{background-position:0 0,0 0,32px 64px,64px 32px}}@keyframes breathe{0%,100%{opacity:.55;filter:drop-shadow(0 0 2px var(--cyan))}50%{opacity:1;filter:drop-shadow(0 0 9px var(--cyan))}}@keyframes slideScan{from{transform:translateY(-120%)}to{transform:translateY(420%)}}@keyframes floatCar{0%,100%{transform:translateY(0)}50%{transform:translateY(-6px)}}@keyframes orbit{to{transform:rotate(360deg)}}@keyframes reveal{from{opacity:0;transform:translateY(13px)}to{opacity:1;transform:translateY(0)}}@keyframes signal{0%,100%{box-shadow:0 0 0 0 rgba(255,205,0,.25)}50%{box-shadow:0 0 0 7px rgba(255,205,0,0)}}
button{font:inherit;cursor:pointer;border:0;color:inherit}button:focus-visible{outline:2px solid var(--lime);outline-offset:3px}.topbar{width:min(1120px,calc(100% - 28px));margin:12px auto 0;display:flex;align-items:center;justify-content:space-between;gap:12px;padding:12px 15px;border:1px solid var(--line);border-radius:15px;background:rgba(8,7,5,.78);backdrop-filter:blur(14px);position:relative;z-index:2}.brand{display:flex;align-items:center;gap:10px;font-size:.79rem;font-weight:900;letter-spacing:.18em}.brand small{display:block;font-size:.56rem;letter-spacing:.18em;color:var(--muted);font-weight:600}.dot{width:9px;height:9px;border-radius:50%;background:var(--lime);animation:signal 1.8s infinite}.dot.offline{background:var(--red);animation:none}.top-actions{display:flex;align-items:center;gap:8px}.tiny-btn{font-size:.68rem;letter-spacing:.08em;font-weight:800;padding:9px 12px;border:1px solid var(--line);background:rgba(255,205,0,.06);border-radius:10px}.tiny-btn:active{transform:scale(.97)}
.shell{width:min(1120px,calc(100% - 28px));margin:18px auto 30px;display:grid;gap:16px}.hero{overflow:hidden;position:relative;min-height:240px;border:1px solid rgba(255,205,0,.32);border-radius:25px;background:linear-gradient(115deg,rgba(27,17,4,.94),rgba(28,8,8,.87));box-shadow:var(--shadow);display:grid;grid-template-columns:1fr .8fr;align-items:center;padding:clamp(20px,4vw,42px);animation:reveal .65s both}.hero:before{content:"";position:absolute;left:0;right:0;top:0;height:2px;background:linear-gradient(90deg,transparent,var(--cyan),var(--pink),transparent);animation:breathe 3s infinite}.hero:after{content:"";position:absolute;left:-30%;top:0;bottom:0;width:23%;background:linear-gradient(90deg,transparent,rgba(255,205,0,.09),transparent);transform:skewX(-20deg);animation:scanAcross 8s linear infinite}@keyframes scanAcross{to{left:135%}}.eyebrow{color:var(--cyan);font-size:.64rem;font-weight:900;letter-spacing:.22em;text-transform:uppercase;display:flex;align-items:center;gap:8px}.eyebrow:before{content:"";width:22px;height:2px;background:var(--cyan);box-shadow:0 0 8px var(--cyan)}h1{font-size:clamp(2rem,5vw,3.8rem);line-height:.99;letter-spacing:-.055em;margin:13px 0 12px;font-weight:950;text-shadow:3px 0 rgba(255,48,48,.55),-2px 0 rgba(255,205,0,.36)}h1 span{color:var(--cyan)}.hero p{max-width:460px;color:#c2b8a0;font-size:.91rem;line-height:1.65}.hero-metrics{display:flex;gap:12px;flex-wrap:wrap;margin-top:20px}.metric-chip{border:1px solid rgba(255,205,0,.18);border-radius:10px;padding:8px 10px;background:rgba(255,205,0,.04);font-size:.65rem;color:var(--muted);letter-spacing:.08em}.metric-chip b{color:var(--lime);display:block;font-size:.8rem;margin-bottom:2px}.car-art{width:100%;max-width:330px;justify-self:center;position:relative;animation:floatCar 4s ease-in-out infinite;filter:drop-shadow(0 0 18px rgba(255,205,0,.2))}.orbit{position:absolute;inset:8% 0 0 8%;width:84%;aspect-ratio:1;border:1px dashed rgba(255,48,48,.34);border-radius:50%;animation:orbit 24s linear infinite}.orbit:before{content:"";position:absolute;left:8%;top:18%;width:7px;height:7px;background:var(--pink);border-radius:50%;box-shadow:0 0 12px var(--pink)}
.grid{display:grid;grid-template-columns:1fr 1.25fr;gap:16px}.panel{position:relative;min-width:0;background:var(--panel);border:1px solid var(--line);border-radius:var(--radius);box-shadow:var(--shadow);padding:18px;overflow:hidden;animation:reveal .6s both}.panel:nth-child(2){animation-delay:.06s}.panel:after{content:"";position:absolute;right:0;top:0;width:75px;height:1px;background:var(--pink);box-shadow:0 0 14px var(--pink)}.panel-head{display:flex;align-items:center;justify-content:space-between;gap:10px;margin-bottom:16px}.panel-title{font-size:.76rem;letter-spacing:.17em;font-weight:950;text-transform:uppercase}.panel-index{font-size:.59rem;color:var(--cyan);letter-spacing:.13em}.telemetry{display:grid;grid-template-columns:1fr 1fr;gap:10px}.tile{padding:14px;border:1px solid rgba(255,255,255,.07);border-radius:13px;background:rgba(255,255,255,.025);min-width:0}.tile-label{font-size:.6rem;color:var(--muted);letter-spacing:.1em;text-transform:uppercase;margin-bottom:8px}.tile-value{font-size:1.27rem;font-weight:900;overflow-wrap:anywhere}.tile-value.cyan{color:var(--cyan)}.tile-value.lime{color:var(--lime)}.tile-value.pink{color:var(--pink)}.tile-sub{font-size:.6rem;color:var(--muted);margin-top:6px}.heap-track{height:4px;background:#30230a;border-radius:20px;overflow:hidden;margin-top:10px}.heap-fill{height:100%;width:40%;background:linear-gradient(90deg,var(--cyan),var(--lime));transition:width .45s}
.speed-row{display:grid;grid-template-columns:repeat(3,1fr);gap:8px;margin-bottom:17px}.speed-btn{border:1px solid rgba(255,205,0,.23);background:rgba(255,205,0,.04);border-radius:12px;padding:12px 6px;text-align:center;font-size:.7rem;font-weight:900;letter-spacing:.1em;transition:transform .16s,border-color .16s,background .16s}.speed-btn small{display:block;font-size:.57rem;color:var(--muted);font-weight:600;letter-spacing:0;margin-top:5px}.speed-btn.active{border-color:var(--lime);background:rgba(255,205,0,.1);color:var(--lime);box-shadow:inset 0 0 16px rgba(255,205,0,.05)}.speed-btn:active{transform:scale(.97)}.drive-layout{display:grid;grid-template-columns:minmax(180px,1fr) minmax(130px,.8fr);align-items:center;gap:14px}.joystick-wrap{display:grid;place-items:center}.joystick{width:min(100%,260px);aspect-ratio:1;border-radius:50%;position:relative;touch-action:none;user-select:none;background:radial-gradient(circle,rgba(255,205,0,.1) 0 22%,rgba(16,11,4,.9) 23% 62%,rgba(255,205,0,.08) 63% 64%,rgba(255,48,48,.05) 65%);border:1px solid rgba(255,205,0,.42);box-shadow:inset 0 0 28px rgba(255,205,0,.08),0 0 25px rgba(255,205,0,.05)}.joystick:before,.joystick:after{content:"";position:absolute;inset:9%;border:1px dashed rgba(255,205,0,.2);border-radius:50%;pointer-events:none}.joystick:after{inset:25%;border-style:solid;border-color:rgba(255,48,48,.2)}.axis{position:absolute;left:50%;top:8%;bottom:8%;width:1px;background:rgba(255,205,0,.18)}.axis.h{top:50%;left:8%;right:8%;bottom:auto;width:auto;height:1px}.direction{position:absolute;color:rgba(255,205,0,.65);font-size:.66rem;font-weight:900}.direction.up{left:50%;top:10%;transform:translateX(-50%)}.direction.down{left:50%;bottom:10%;transform:translateX(-50%)}.direction.left{left:10%;top:50%;transform:translateY(-50%)}.direction.right{right:10%;top:50%;transform:translateY(-50%)}.stick{position:absolute;left:50%;top:50%;width:31%;height:31%;transform:translate(-50%,-50%);border-radius:50%;border:2px solid var(--cyan);background:radial-gradient(circle at 35% 30%,#fff5c7,#d38b00 43%,#311007 75%);box-shadow:0 0 20px rgba(255,205,0,.3);z-index:2;pointer-events:none;transition:box-shadow .15s}.stick.active{box-shadow:0 0 30px rgba(255,205,0,.75),0 0 55px rgba(255,48,48,.18)}.drive-hints{display:grid;gap:9px}.hint{display:flex;gap:9px;align-items:flex-start;padding:10px;border-radius:10px;background:rgba(255,255,255,.025);font-size:.66rem;color:var(--muted);line-height:1.45}.hint b{display:block;color:var(--text);font-size:.7rem;margin-bottom:2px}.hint-icon{flex:0 0 24px;width:24px;height:24px;border-radius:7px;background:rgba(255,205,0,.08);color:var(--cyan);display:grid;place-items:center;font-size:.7rem;font-weight:900}.status-pill{display:inline-flex;align-items:center;gap:6px;border-radius:20px;padding:5px 8px;border:1px solid rgba(255,205,0,.22);font-size:.58rem;color:var(--lime);letter-spacing:.08em}.status-pill.danger{border-color:rgba(255,52,90,.5);color:var(--red);background:rgba(255,52,90,.08)}
.actions{display:grid;grid-template-columns:1fr 1fr;gap:9px}.action{min-height:45px;padding:12px;border-radius:11px;border:1px solid rgba(255,205,0,.22);background:rgba(255,205,0,.05);font-size:.67rem;font-weight:900;letter-spacing:.08em}.action:active{transform:scale(.98)}.action.release{border-color:rgba(255,176,0,.3);color:var(--amber)}.action.danger{grid-column:1/-1;background:linear-gradient(105deg,rgba(255,52,90,.16),rgba(255,48,48,.1));border-color:rgba(255,52,90,.7);color:#ff716b;min-height:54px;box-shadow:0 0 20px rgba(255,52,90,.06)}.action.resume{grid-column:1/-1;border-color:rgba(255,205,0,.4);color:var(--lime);background:rgba(255,205,0,.07)}.action:disabled{opacity:.45;cursor:not-allowed}.footer{color:#8c7c5b;text-align:center;font-size:.61rem;line-height:1.7;padding:10px 0 20px;letter-spacing:.04em}.footer b{color:var(--cyan);font-weight:800}#toast{position:fixed;z-index:20;bottom:22px;left:50%;transform:translate(-50%,130px);opacity:0;max-width:calc(100% - 30px);background:#171006;border:1px solid var(--cyan);box-shadow:0 0 24px rgba(255,205,0,.16);border-radius:12px;padding:12px 17px;text-align:center;font-size:.73rem;font-weight:800;transition:all .25s;pointer-events:none}#toast.show{transform:translate(-50%,0);opacity:1}
@media(max-width:780px){.hero{grid-template-columns:1fr;gap:12px;min-height:unset}.car-art{max-width:240px;grid-row:1}.hero-copy{grid-row:2}.grid{grid-template-columns:1fr}.drive-layout{grid-template-columns:minmax(150px,1fr) minmax(115px,.8fr)}.panel{padding:15px}}
@media(max-width:420px){.shell,.topbar{width:calc(100% - 18px)}.hero{padding:20px}.hero p{font-size:.82rem}.drive-layout{grid-template-columns:1fr}.joystick{width:min(100%,245px)}.drive-hints{grid-template-columns:1fr 1fr}.telemetry{gap:7px}.tile{padding:11px}.tile-value{font-size:1.05rem}.brand{letter-spacing:.1em;font-size:.7rem}}

/* SPARK team + low-memory robot expression */
.hero-visual{display:grid;grid-template-columns:minmax(0,1fr) 132px;gap:10px;align-items:center;min-width:0}
.face-card{border:1px solid rgba(255,48,48,.32);border-radius:15px;background:linear-gradient(160deg,rgba(18,12,4,.92),rgba(38,8,5,.72));padding:10px;min-width:0;text-align:center;position:relative;overflow:hidden}
.face-card:before{content:"";position:absolute;inset:0;background:linear-gradient(transparent 49%,rgba(255,205,0,.06) 50%,transparent 51%);background-size:100% 5px;pointer-events:none}
.face-kicker{font-size:.5rem;letter-spacing:.12em;color:var(--pink);font-weight:900;position:relative}
.robot-face{width:100%;max-width:240px;height:108px;object-fit:contain;display:block;margin:7px auto 5px;border-radius:10px;filter:drop-shadow(0 0 8px rgba(255,205,0,.22));position:relative;transition:filter .2s,transform .2s}
.robot-face[data-mood="forward"],.robot-face[data-mood="fast"]{filter:drop-shadow(0 0 11px rgba(255,205,0,.44));transform:scale(1.02)}
.robot-face[data-mood="brake"],.robot-face[data-mood="error"]{filter:drop-shadow(0 0 11px rgba(255,48,48,.5))}
.face-label{font-size:.56rem;font-weight:950;letter-spacing:.1em;color:var(--cyan);position:relative;overflow-wrap:anywhere}
.team-panel{border:1px solid rgba(255,48,48,.35);background:linear-gradient(115deg,rgba(18,12,4,.95),rgba(12,10,5,.95));border-radius:var(--radius);padding:18px;box-shadow:var(--shadow);position:relative;overflow:hidden}
.team-panel:before{content:"SPARK // ONE TEAM";position:absolute;right:18px;top:16px;font-size:.55rem;letter-spacing:.18em;color:var(--pink);opacity:.8}
.team-intro{font-size:.75rem;color:var(--muted);margin:-5px 0 15px;line-height:1.6;max-width:700px}
.team-grid{display:grid;grid-template-columns:repeat(4,minmax(0,1fr));gap:11px}
.team-card{border:1px solid rgba(255,205,0,.2);border-radius:13px;background:rgba(0,0,0,.18);padding:14px 10px;text-align:center;min-width:0;position:relative;transition:transform .2s,border-color .2s}
.team-card:nth-child(2){border-color:rgba(255,205,0,.38)}.team-card:nth-child(3){border-color:rgba(255,205,0,.32)}.team-card:nth-child(4){border-color:rgba(255,48,48,.38)}
.team-card:hover{transform:translateY(-3px);border-color:var(--cyan)}
.team-avatar{width:62px;height:62px;margin:0 auto 10px;border-radius:18px 7px 18px 7px;display:grid;place-items:center;font-size:1.2rem;font-weight:950;letter-spacing:.04em;color:var(--cyan);background:radial-gradient(circle at 30% 20%,rgba(255,205,0,.2),rgba(18,12,4,.9) 70%);border:1px solid currentColor;box-shadow:inset 0 0 18px rgba(255,205,0,.08),0 0 13px rgba(255,205,0,.06)}
.team-card:nth-child(2) .team-avatar{color:var(--cyan)}.team-card:nth-child(3) .team-avatar{color:var(--lime)}.team-card:nth-child(4) .team-avatar{color:var(--pink)}
.team-name{font-size:.88rem;font-weight:950;letter-spacing:.02em;overflow-wrap:anywhere}.team-role{color:var(--cyan);font-size:.67rem;font-weight:850;margin-top:6px}.team-meta{color:var(--muted);font-size:.59rem;line-height:1.45;margin-top:5px}
.auth-gate{display:flex;position:fixed;inset:0;z-index:50;align-items:center;justify-content:center;padding:20px;background:rgba(3,5,14,.91);backdrop-filter:blur(15px)}.auth-gate.hidden{display:none}.auth-box{width:min(420px,100%);border:1px solid rgba(255,205,0,.55);border-radius:22px;background:linear-gradient(145deg,#160d05,#2b0808);padding:25px;box-shadow:0 0 45px rgba(255,205,0,.1),0 0 55px rgba(255,48,48,.08);position:relative;overflow:hidden}.auth-box:before{content:"";position:absolute;left:0;right:0;top:0;height:3px;background:linear-gradient(90deg,var(--cyan),var(--pink),var(--lime))}.auth-kicker{color:var(--cyan);font-size:.62rem;letter-spacing:.2em;font-weight:950}.auth-box h2{font-size:1.65rem;margin:12px 0 8px;letter-spacing:-.03em}.auth-box p{font-size:.75rem;line-height:1.65;color:var(--muted);margin-bottom:15px}.auth-box label{display:block;font-size:.63rem;letter-spacing:.12em;color:var(--cyan);font-weight:900;margin:10px 0 7px}.auth-box input{width:100%;padding:14px;border-radius:10px;border:1px solid rgba(255,205,0,.3);background:#100707;color:var(--text);font:inherit;letter-spacing:.2em;outline:none}.auth-box input:focus{border-color:var(--lime);box-shadow:0 0 0 2px rgba(255,205,0,.1)}.auth-submit{width:100%;margin-top:12px;padding:13px;border-radius:10px;background:linear-gradient(95deg,rgba(255,205,0,.2),rgba(255,48,48,.18));border:1px solid var(--cyan);font-size:.72rem;font-weight:950;letter-spacing:.12em}.auth-error{min-height:1.2em;color:#ff716b!important;margin:10px 0 0!important;font-size:.7rem!important}
@media(max-width:780px){.hero-visual{width:min(100%,520px);justify-self:center}.team-grid{grid-template-columns:repeat(2,minmax(0,1fr))}.team-panel:before{position:static;display:block;margin-bottom:8px;text-align:right}}
@media(max-width:420px){.hero-visual{grid-template-columns:minmax(0,1fr) 104px;gap:7px}.face-card{padding:7px}.face-kicker{font-size:.44rem}.face-label{font-size:.48rem}.team-grid{gap:8px}.team-card{padding:12px 7px}.team-avatar{width:53px;height:53px;font-size:1rem}.team-name{font-size:.78rem}.team-meta{font-size:.55rem}}


.mode-wall{border-color:rgba(255,205,0,.30);background:linear-gradient(115deg,rgba(20,14,5,.96),rgba(25,7,7,.94))}
.mode-grid{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:10px}
.mode-card{min-width:0;display:grid;grid-template-columns:66px minmax(0,1fr);align-items:center;gap:10px;border:1px solid rgba(255,48,48,.24);border-radius:13px;background:rgba(0,0,0,.35);padding:10px;position:relative}
.mode-card.live{border-color:rgba(255,205,0,.56);box-shadow:inset 0 0 18px rgba(255,205,0,.06)}
.mode-card img{width:66px;height:54px;object-fit:contain;border-radius:7px;background:#030303}
.mode-card b{display:block;color:var(--text);font-size:.65rem;letter-spacing:.06em;margin-bottom:4px}
.mode-card small{display:block;color:var(--muted);font-size:.58rem;line-height:1.4}
.mode-tag{justify-self:start;font-size:.48rem;letter-spacing:.12em;font-weight:950;color:#ffd000;border:1px solid rgba(255,205,0,.38);padding:3px 6px;border-radius:6px;grid-column:2}
.mode-tag.planned{color:#ff6661;border-color:rgba(255,48,48,.38)}
.boot-face{display:block;width:88px;height:58px;object-fit:contain;margin:0 auto 8px;background:#030303;border:1px solid rgba(255,205,0,.22);border-radius:10px}
@media(max-width:780px){.mode-grid{grid-template-columns:1fr}}

@media(prefers-reduced-motion:reduce){*,*:before,*:after{animation-duration:.01ms!important;animation-iteration-count:1!important;transition-duration:.01ms!important;scroll-behavior:auto!important}}
</style>
</head>
<body>
<header class="topbar">
  <div class="brand"><span class="dot" id="statusDot"></span><div>SPARK TEAM / CYBERCAR<small id="topSub">LOCAL ROBOT CONTROL LINK</small></div></div>
  <div class="top-actions"><span class="status-pill" id="linkState">LINK ONLINE</span><button class="tiny-btn" id="langBtn" type="button">AR / EN</button></div>
</header>
<main class="shell">
  <section class="hero">
    <div class="hero-copy"><div class="eyebrow" id="heroTag">EDGE ROBOTICS SYSTEM // 01</div><h1 id="heroTitle">CYBER<span>CAR</span><br>REDLINE DRIVE</h1><p id="heroText">A lightweight, offline-first command deck for your ESP32-S2 robot. Direct Wi-Fi. Live status. One-tap emergency brake.</p><div class="hero-metrics"><div class="metric-chip"><b>ESP32-S2</b><span id="chipBoard">EDGE CONTROLLER</span></div><div class="metric-chip"><b>192.168.4.1</b><span id="chipNetwork">LOCAL CONTROL</span></div><div class="metric-chip"><b id="heapHero">-- KB</b><span id="chipMemory">FREE HEAP</span></div></div></div>
    <div class="hero-visual">    <div class="car-art" aria-label="Animated cyberpunk car illustration" role="img"><div class="orbit"></div><svg viewBox="0 0 320 210" width="100%" xmlns="http://www.w3.org/2000/svg" aria-hidden="true"><defs><linearGradient id="bodyGrad" x1="0" x2="1"><stop stop-color="#291708"/><stop offset=".5" stop-color="#8d250d"/><stop offset="1" stop-color="#4a0909"/></linearGradient><linearGradient id="glass" x1="0" y1="0" x2="1" y2="1"><stop stop-color="#fff0ad"/><stop offset="1" stop-color="#ffd000" stop-opacity=".1"/></linearGradient><filter id="glow"><feGaussianBlur stdDeviation="3" result="blur"/><feMerge><feMergeNode in="blur"/><feMergeNode in="SourceGraphic"/></feMerge></filter></defs><ellipse cx="160" cy="176" rx="112" ry="12" fill="#ffd000" opacity=".12"/><g filter="url(#glow)" stroke="#ffd000" stroke-width="2" stroke-linejoin="round"><path d="M49 140 L66 112 Q73 104 101 99 L125 67 Q132 58 152 58 L208 65 Q223 69 241 101 L268 116 L282 143 L273 158 L42 158 L36 147 Z" fill="url(#bodyGrad)"/><path d="M84 108 L126 71 Q132 66 151 67 L177 69 L180 108 Z" fill="url(#glass)"/><path d="M188 70 L207 74 Q219 79 234 107 L188 108 Z" fill="url(#glass)"/><path d="M45 139 L80 135 L76 147 L40 148 Z" fill="#ff3030" stroke="#ff3030"/><path d="M247 129 L277 139 L278 148 L250 146 Z" fill="#ff3030" stroke="#ff3030"/><path d="M91 118 L232 118" stroke="#ffd000" stroke-width="2"/><path d="M105 132 L219 132" stroke="#ffd000" stroke-width="2" opacity=".7"/><path d="M142 68 L151 108" stroke="#ffd000" opacity=".6"/></g><g fill="#050505" stroke="#ffd000" stroke-width="3"><circle cx="87" cy="157" r="22"/><circle cx="237" cy="157" r="22"/></g><g fill="none" stroke="#ff3030" stroke-width="2"><circle cx="87" cy="157" r="10"/><circle cx="237" cy="157" r="10"/><path d="M87 147 V167 M77 157 H97 M237 147 V167 M227 157 H247"/></g><path d="M53 187 H267" stroke="#ffd000" stroke-width="1" stroke-dasharray="4 7" opacity=".6"/><circle cx="53" cy="187" r="3" fill="#ffd000"/><circle cx="267" cy="187" r="3" fill="#ff3030"/></svg></div>
      <div class="face-card" aria-label="Robot face display reflecting the current command">
        <div class="face-kicker">SPARK BOT / MOOD CORE</div>
        <img id="robotFace" class="robot-face" data-mood="idle" src="/assets/faces/idle.jpg" width="240" height="160" loading="eager" decoding="async" alt="SPARK robot face: idle" />
        <div id="faceLabel" class="face-label">FACE / IDLE</div>
      </div>
    </div>
  </section>
  <section class="grid">
    <article class="panel"><div class="panel-head"><h2 class="panel-title" id="telemetryTitle">SYSTEM TELEMETRY</h2><span class="panel-index">SYS / 01</span></div><div class="telemetry">
      <div class="tile"><div class="tile-label" id="cmdLabel">MOTOR COMMAND</div><div class="tile-value cyan" id="commandValue">STOP</div><div class="tile-sub" id="cmdSub">Awaiting input</div></div>
      <div class="tile"><div class="tile-label" id="speedLabel">DRIVE PROFILE</div><div class="tile-value lime" id="levelValue">MEDIUM</div><div class="tile-sub" id="levelSub">Digital burst profile</div></div>
      <div class="tile"><div class="tile-label" id="memoryLabel">FREE MEMORY</div><div class="tile-value" id="heapValue">-- KB</div><div class="heap-track"><div class="heap-fill" id="heapFill"></div></div><div class="tile-sub" id="heapSub">Runtime free heap</div></div>
      <div class="tile"><div class="tile-label" id="uptimeLabel">UPTIME</div><div class="tile-value pink" id="uptimeValue">00:00</div><div class="tile-sub" id="servoState">SERVO: LOCKED</div></div>
    </div></article>
    <article class="panel"><div class="panel-head"><h2 class="panel-title" id="driveTitle">NEURAL DRIVE DECK</h2><span class="panel-index">DRV / 02</span></div>
      <div class="speed-row" role="group" aria-label="Drive level"><button class="speed-btn" data-level="slow" type="button"><span id="slowLabel">SLOW</span><small id="slowSub">LOW BURST</small></button><button class="speed-btn active" data-level="medium" type="button"><span id="mediumLabel">MEDIUM</span><small id="mediumSub">BALANCED</small></button><button class="speed-btn" data-level="fast" type="button"><span id="fastLabel">FAST</span><small id="fastSub">FULL POWER</small></button></div>
      <div class="drive-layout"><div class="joystick-wrap"><div class="joystick" id="joystick" aria-label="Touch and drag to drive"><span class="axis"></span><span class="axis h"></span><span class="direction up">▲</span><span class="direction down">▼</span><span class="direction left">◀</span><span class="direction right">▶</span><div class="stick" id="stick"></div></div></div><div class="drive-hints"><div class="hint"><span class="hint-icon">↟</span><div><b id="hintForward">Push to move</b><span id="hintForwardSub">Hold and drag; release to stop.</span></div></div><div class="hint"><span class="hint-icon">!</span><div><b id="hintSafety">Dead-man timeout</b><span id="hintSafetySub">The motors stop if commands disappear.</span></div></div><div class="hint"><span class="hint-icon">◇</span><div><b id="hintLocal">Local Wi-Fi</b><span id="hintLocalSub">Works without a router or cloud.</span></div></div></div></div>
    </article>
  </section>
  <section class="panel"><div class="panel-head"><h2 class="panel-title" id="actionTitle">AUXILIARY SYSTEMS</h2><span class="panel-index">ACT / 03</span></div><div class="actions">
    <button class="action release" id="releaseBtn" type="button">RELEASE BALL</button><button class="action" id="lockBtn" type="button">LOCK BALL</button><button class="action" id="hornBtn" type="button">SOUND SIGNAL</button>
    <button class="action danger" id="brakeBtn" type="button">■ EMERGENCY BRAKE — LATCHED STOP</button><button class="action resume" id="resumeBtn" type="button" disabled>↻ RESUME DRIVE</button>
  </div></section>

  <section class="panel mode-wall" aria-label="SPARK image states and AI mode concepts">
    <div class="panel-head"><h2 class="panel-title">SPARK CONTROL PROFILES</h2><span class="panel-index">VISUAL / 05</span></div>
    <div class="mode-grid">
      <article class="mode-card live"><img src="/assets/faces/manual.jpg" alt="Manual mode face" loading="lazy" decoding="async"><div><b>MANUAL / LIVE</b><small>Implemented joystick control</small></div><span class="mode-tag">ACTIVE</span></article>
      <article class="mode-card"><img src="/assets/faces/auto.jpg" alt="Autonomous mode concept face" loading="lazy" decoding="async"><div><b>AUTO / CONCEPT</b><small>Visual concept only; autonomy is not enabled in this firmware</small></div><span class="mode-tag planned">LAB</span></article>
      <article class="mode-card"><img src="/assets/faces/qlearning.jpg" alt="Q-learning concept face" loading="lazy" decoding="async"><div><b>Q-LEARNING / CONCEPT</b><small>Visual concept only; no trained policy is running</small></div><span class="mode-tag planned">LAB</span></article>
    </div>
  </section>
  <section class="team-panel" id="sparkTeam">
    <div class="panel-head"><h2 class="panel-title" id="teamTitle">SPARK TEAM / THE PEOPLE BEHIND THE CAR</h2><span class="panel-index">TEAM / 04</span></div>
    <p class="team-intro" id="teamSub">Four engineering roles. One shared mission: build a smarter, safer, lighter robot and document every decision.</p>
    <div class="team-grid">
      <article class="team-card"><div class="team-avatar" aria-hidden="true">HN</div><h3 class="team-name">Haider Nather</h3><div class="team-role">Team Lead</div><div class="team-meta">AI &amp; Robotics Engineer</div></article>
      <article class="team-card"><div class="team-avatar" aria-hidden="true">AA</div><h3 class="team-name">Ahmed Amjed</h3><div class="team-role">AI Developer</div><div class="team-meta">AI &amp; Robotics Engineer</div></article>
      <article class="team-card"><div class="team-avatar" aria-hidden="true">AS</div><h3 class="team-name">Ayat Sayed</h3><div class="team-role">System Designer</div><div class="team-meta">AI &amp; Robotics Engineer</div></article>
      <article class="team-card"><div class="team-avatar" aria-hidden="true">BF</div><h3 class="team-name">Baraa Fadhel</h3><div class="team-role">Robotics Designer</div><div class="team-meta">AI &amp; Robotics Engineer</div></article>
    </div>
  </section>
  <div class="footer"><b>SPARK TEAM // CYBERCAR REDLINE</b> — <span id="footerText">Built for offline robotics experiments. Test with wheels lifted before floor driving.</span><br><span id="speedNote">SLOW / MEDIUM are coarse ON/OFF bursts and may feel jerky; FAST is continuously enabled.</span></div>
</main>
<div class="auth-gate" id="authGate" role="dialog" aria-modal="true" aria-labelledby="authTitle">
  <div class="auth-box"><img class="boot-face" src="/assets/faces/boot.jpg" alt="SPARK boot face" width="88" height="58"><div class="auth-kicker">SPARK TEAM // SECURE LOCAL ACCESS</div><h2 id="authTitle">UNLOCK CONTROL</h2><p>The dashboard is protected by a local control PIN. Opening the captive portal alone does not grant permission to drive.</p>
    <form id="loginForm"><label for="pinInput">CONTROL PIN</label><input id="pinInput" name="pin" type="password" inputmode="numeric" autocomplete="current-password" maxlength="16" required aria-describedby="loginError"><button class="auth-submit" type="submit">AUTHENTICATE &amp; CONNECT</button><p id="loginError" class="auth-error" role="status" aria-live="polite"></p></form>
  </div>
</div>
<div id="toast" role="status" aria-live="polite"></div>
<script>
'use strict';
const byId = id => document.getElementById(id);
let lang = 'en', busyState = false, toastTimer = 0, dragging = false, lastSent = 0;
let joyX = 0, joyY = 0, localEstop = false, activeLevel = 'medium';
const dict = {
 en:{topSub:'LOCAL ROBOT CONTROL LINK',linkOn:'LINK ONLINE',linkOff:'LINK LOST',heroTag:'EDGE ROBOTICS SYSTEM // 01',heroText:'A lightweight, offline-first command deck for your ESP32-S2 robot. Direct Wi-Fi. Live status. One-tap emergency brake.',chipBoard:'EDGE CONTROLLER',chipNetwork:'LOCAL CONTROL',chipMemory:'FREE HEAP',telemetryTitle:'SYSTEM TELEMETRY',cmdLabel:'MOTOR COMMAND',cmdSub:'Live H-bridge command',speedLabel:'DRIVE PROFILE',levelSub:'Digital ON/OFF burst',memoryLabel:'FREE MEMORY',heapSub:'Runtime free heap',uptimeLabel:'UPTIME',driveTitle:'NEURAL DRIVE DECK',slowLabel:'SLOW',slowSub:'LOW BURST',mediumLabel:'MEDIUM',mediumSub:'BALANCED',fastLabel:'FAST',fastSub:'FULL POWER',hintForward:'Push to move',hintForwardSub:'Hold and drag; release to stop.',hintSafety:'Dead-man timeout',hintSafetySub:'Motors stop if commands disappear.',hintLocal:'Local Wi-Fi',hintLocalSub:'No router or cloud needed.',actionTitle:'AUXILIARY SYSTEMS',teamTitle:'SPARK TEAM / THE PEOPLE BEHIND THE CAR',teamSub:'Four engineering roles. One shared mission: build a smarter, safer, lighter robot and document every decision.',releaseBtn:'RELEASE BALL',lockBtn:'LOCK BALL',hornBtn:'SOUND SIGNAL',brakeBtn:'■ EMERGENCY BRAKE — LATCHED STOP',resumeBtn:'↻ RESUME DRIVE',footerText:'Built for offline robotics experiments. Test with wheels lifted before floor driving.',speedNote:'SLOW / MEDIUM use coarse ON/OFF bursts and may feel jerky; FAST stays continuously enabled.',locked:'SERVO: LOCKED',released:'SERVO: RELEASED',estop:'EMERGENCY STOP LATCHED',ready:'READY / TOUCH TO DRIVE',connecting:'CONNECTING…',online:'Connected to the car.',offline:'Connection lost. Check Wi-Fi.',braked:'Emergency brake latched. Press RESUME to unlock.',resumed:'Drive unlocked. Car remains stopped.',sent:'Command accepted.',speed:'Drive profile updated.',action:'Action sent.',denied:'Command refused. Resume after emergency stop.',bad:'Request failed. Try again.',up:'MOVE',down:'MOVE',left:'MOVE',right:'MOVE',stop:'STOP'},
 ar:{topSub:'رابط التحكم المحلي بالروبوت',linkOn:'الاتصال يعمل',linkOff:'انقطع الاتصال',heroTag:'نظام روبوتات طرفي // 01',heroText:'واجهة تحكم خفيفة تعمل دون إنترنت لسيارتك ESP32-S2. اتصال مباشر عبر Wi-Fi، حالة مباشرة، وزر فرملة طارئة.',chipBoard:'وحدة التحكم',chipNetwork:'تحكم محلي',chipMemory:'ذاكرة حرة',telemetryTitle:'بيانات النظام',cmdLabel:'أمر المحركات',cmdSub:'أمر المحرك الحالي',speedLabel:'مستوى الحركة',levelSub:'تشغيل وإيقاف رقمي متقطع',memoryLabel:'الذاكرة الحرة',heapSub:'ذاكرة وقت التشغيل',uptimeLabel:'مدة التشغيل',driveTitle:'لوحة قيادة سايبر',slowLabel:'بطيء',slowSub:'نبضات قليلة',mediumLabel:'متوسط',mediumSub:'متوازن',fastLabel:'سريع',fastSub:'طاقة كاملة',hintForward:'حرّك العصا',hintForwardSub:'اسحب باستمرار؛ اتركها للتوقف.',hintSafety:'توقف عند انقطاع الأوامر',hintSafetySub:'تتوقف المحركات عند فقد الأوامر.',hintLocal:'شبكة محلية',hintLocalSub:'لا تحتاج إلى راوتر أو سحابة.',actionTitle:'الأنظمة المساعدة',teamTitle:'فريق سبارك / العقول خلف السيارة',teamSub:'أربعة أدوار هندسية ومهمة واحدة: روبوت أذكى وأكثر أماناً وأخف وزناً مع توثيق كل قرار.',releaseBtn:'تحرير الكرة',lockBtn:'قفل الكرة',hornBtn:'إشارة صوتية',brakeBtn:'■ فرملة طارئة — إيقاف مقفل',resumeBtn:'↻ استئناف التحكم',footerText:'مصمم لتجارب الروبوتات دون إنترنت. ارفع العجلات قبل اختبار الحركة.',speedNote:'البطيء والمتوسط يعتمدان نبضات تشغيل/إيقاف وقد تكون الحركة متقطعة؛ السريع يعمل باستمرار.',locked:'المؤازر: مقفل',released:'المؤازر: محرر',estop:'الفرملة الطارئة مفعلة',ready:'جاهز / المس للتحكم',connecting:'جارٍ الاتصال…',online:'تم الاتصال بالسيارة.',offline:'انقطع الاتصال. تحقق من Wi-Fi.',braked:'تم قفل الفرملة. اضغط استئناف لإلغاء القفل.',resumed:'أُلغي قفل القيادة. السيارة لا تزال متوقفة.',sent:'تم قبول الأمر.',speed:'تم تحديث مستوى الحركة.',action:'تم إرسال الأمر.',denied:'رُفض الأمر. ألغِ قفل الفرملة أولاً.',bad:'فشل الطلب. حاول مجدداً.',up:'حركة',down:'حركة',left:'حركة',right:'حركة',stop:'توقف'}
};
function showToast(msg){const el=byId('toast');clearTimeout(toastTimer);el.textContent=msg;el.classList.add('show');toastTimer=setTimeout(()=>el.classList.remove('show'),2300)}
function translate(){const t=dict[lang];document.documentElement.lang=lang;document.documentElement.dir=lang==='ar'?'rtl':'ltr';Object.keys(t).forEach(k=>{const el=byId(k);if(el)el.textContent=t[k]});byId('langBtn').textContent=lang==='en'?'AR / EN':'EN / AR';}
byId('langBtn').addEventListener('click',()=>{lang=lang==='en'?'ar':'en';translate()});translate();
function showAuthGate(){const gate=byId('authGate');if(gate.classList.contains('hidden')){gate.classList.remove('hidden');setTimeout(()=>byId('pinInput').focus(),50)}}
function hideAuthGate(){byId('authGate').classList.add('hidden');byId('loginError').textContent=''}
byId('loginForm').addEventListener('submit',async e=>{
  e.preventDefault();const pin=byId('pinInput').value;
  if(!pin || pin.length>16){byId('loginError').textContent='Enter the control PIN.';return}
  const body=new URLSearchParams({pin});
  try{
    const r=await fetch('/api/login',{method:'POST',credentials:'same-origin',cache:'no-store',headers:{'Content-Type':'application/x-www-form-urlencoded;charset=UTF-8'},body:body.toString()});
    let d={};try{d=await r.json()}catch(_e){}
    if(!r.ok)throw new Error(d.error||'login_failed');
    byId('pinInput').value='';hideAuthGate();showToast('Spark session authenticated.');refreshState();
  }catch(err){byId('loginError').textContent=err.message==='invalid_pin'?'Incorrect PIN. Try again.':(err.message==='login_locked'?'Too many attempts. Wait 30 seconds.':'Login failed. Check the PIN and reconnect.');byId('pinInput').select()}
});
const faceAssets={idle:'/assets/faces/idle.jpg',forward:'/assets/faces/happy.jpg',backward:'/assets/faces/brake.jpg',left:'/assets/faces/dance.jpg',right:'/assets/faces/drift.jpg',fast:'/assets/faces/dance2.jpg',brake:'/assets/faces/angry.jpg',error:'/assets/faces/boot.jpg'};
function setRobotFace(command, estop=false){
  const moving=['forward','backward','left','right'].includes(command);
  let mood=estop||command==='brake'?'brake':(moving?command:(command==='error'?'error':'idle'));
  if(moving && activeLevel==='fast' && command==='forward')mood='fast';
  const face=byId('robotFace');
  if(face){
    face.dataset.mood=mood;
    const next=faceAssets[mood]||faceAssets.idle;
    if(face.dataset.asset!==next){face.dataset.asset=next;face.src=next;}
    face.alt='SPARK robot face: '+mood;
  }
  const labels={idle:'FACE / IDLE',forward:'FACE / FORWARD',backward:'FACE / REVERSE',left:'FACE / LEFT',right:'FACE / RIGHT',fast:'FACE / BOOST',brake:'FACE / E-STOP',error:'FACE / ERROR'};
  const label=byId('faceLabel');if(label)label.textContent=labels[mood]||labels.idle;
}
function commandFromXY(x,y){if(Math.abs(x)>Math.abs(y))return x>22?'right':(x< -22?'left':'stop');return y>22?'forward':(y< -22?'backward':'stop')}
async function api(path,method='POST'){const r=await fetch(path,{method,credentials:'same-origin',cache:'no-store'});let data={};try{data=await r.json()}catch(_e){}if(!r.ok){const err=new Error(data.error||'request_failed');err.status=r.status;throw err}return data}
function labelLevel(level){activeLevel=level;document.querySelectorAll('.speed-btn').forEach(b=>b.classList.toggle('active',b.dataset.level===level));byId('levelValue').textContent=level.toUpperCase();setRobotFace(commandFromXY(joyX,joyY),localEstop)}
document.querySelectorAll('.speed-btn').forEach(b=>b.addEventListener('click',async()=>{try{await api('/api/speed?level='+b.dataset.level);labelLevel(b.dataset.level);showToast(dict[lang].speed)}catch(_e){showToast(dict[lang].bad)}}));
function drawStick(dx,dy){byId('stick').style.transform=`translate(calc(-50% + ${dx}px),calc(-50% + ${dy}px))`;byId('stick').classList.toggle('active',dragging)}
function resetJoystick(sendStop=true){dragging=false;joyX=0;joyY=0;drawStick(0,0);setRobotFace('stop',localEstop);if(sendStop)sendJoy(0,0)}
async function sendJoy(x,y){joyX=x;joyY=y;lastSent=Date.now();setRobotFace(commandFromXY(x,y),localEstop);try{await api('/api/joy?x='+x+'&y='+y)}catch(e){if(e.message==='emergency_stop_latched')showToast(dict[lang].denied)}}
function updateJoystick(e){if(!dragging)return;const el=byId('joystick'),r=el.getBoundingClientRect();let dx=e.clientX-(r.left+r.width/2),dy=e.clientY-(r.top+r.height/2);const max=r.width*.30,d=Math.hypot(dx,dy);if(d>max){dx=dx/d*max;dy=dy/d*max}drawStick(dx,dy);joyX=Math.round(dx/max*100);joyY=Math.round(-dy/max*100);if(Date.now()-lastSent>80)sendJoy(joyX,joyY)}
byId('joystick').addEventListener('pointerdown',e=>{if(localEstop)return;dragging=true;byId('joystick').setPointerCapture(e.pointerId);updateJoystick(e)});
byId('joystick').addEventListener('pointermove',updateJoystick);
byId('joystick').addEventListener('pointerup',()=>resetJoystick());
byId('joystick').addEventListener('pointercancel',()=>resetJoystick());
byId('joystick').addEventListener('lostpointercapture',()=>{if(dragging)resetJoystick()});
window.addEventListener('blur',()=>resetJoystick());document.addEventListener('visibilitychange',()=>{if(document.hidden)resetJoystick()});
setInterval(()=>{if(dragging&&Date.now()-lastSent>135)sendJoy(joyX,joyY)},160);
byId('brakeBtn').addEventListener('click',async()=>{resetJoystick(false);localEstop=true;try{await api('/api/brake');updateEstop(true);showToast(dict[lang].braked)}catch(_e){showToast(dict[lang].bad)}});
byId('resumeBtn').addEventListener('click',async()=>{try{await api('/api/resume');localEstop=false;updateEstop(false);resetJoystick(false);showToast(dict[lang].resumed)}catch(_e){showToast(dict[lang].bad)}});
function updateEstop(active){localEstop=active;byId('resumeBtn').disabled=!active;byId('brakeBtn').classList.add('danger');byId('linkState').classList.toggle('danger',active);if(active){resetJoystick(false);byId('commandValue').textContent='BRAKE';byId('commandValue').classList.remove('cyan');byId('commandValue').classList.add('pink')}setRobotFace(active?'brake':'stop',active)}
byId('releaseBtn').addEventListener('click',async()=>{try{await api('/api/ball?action=release');showToast(dict[lang].action)}catch(_e){showToast(dict[lang].bad)}});
byId('lockBtn').addEventListener('click',async()=>{try{await api('/api/ball?action=lock');showToast(dict[lang].action)}catch(_e){showToast(dict[lang].bad)}});
byId('hornBtn').addEventListener('click',async()=>{try{await api('/api/horn');showToast(dict[lang].action)}catch(_e){showToast(dict[lang].bad)}});
function fmtUptime(s){s=Math.max(0,Math.floor(s||0));const h=Math.floor(s/3600),m=Math.floor((s%3600)/60),sec=s%60;return h?`${String(h).padStart(2,'0')}:${String(m).padStart(2,'0')}:${String(sec).padStart(2,'0')}`:`${String(m).padStart(2,'0')}:${String(sec).padStart(2,'0')}`}
async function refreshState(){if(busyState)return;busyState=true;try{const d=await api('/api/state','GET');byId('statusDot').classList.remove('offline');byId('linkState').textContent=dict[lang].linkOn;byId('commandValue').textContent=(d.command||'stop').toUpperCase();byId('commandValue').classList.toggle('pink',!!d.estop);byId('commandValue').classList.toggle('cyan',!d.estop);labelLevel(d.level||'medium');const heap=Math.max(0,Number(d.heap)||0),kb=Math.round(heap/1024);byId('heapValue').textContent=kb+' KB';byId('heapHero').textContent=kb+' KB';const ratio=Math.min(100,heap/220000*100);byId('heapFill').style.width=ratio+'%';byId('uptimeValue').textContent=fmtUptime(d.uptime);byId('servoState').textContent=d.ball==='locked'?dict[lang].locked:dict[lang].released;updateEstop(!!d.estop);setRobotFace(d.command||'stop',!!d.estop);hideAuthGate()}catch(e){if(e.status===403 && e.message==='open_dashboard_first'){showAuthGate()}else{byId('statusDot').classList.add('offline');byId('linkState').textContent=dict[lang].linkOff;showToastOnce()}}finally{busyState=false}}
let offlineShown=false;function showToastOnce(){if(!offlineShown){showToast(dict[lang].offline);offlineShown=true}setTimeout(()=>{offlineShown=false},4500)}
setRobotFace('stop',false);showAuthGate();refreshState();setInterval(refreshState,1000);
</script>
</body>
</html>
)CYBERHTML";
