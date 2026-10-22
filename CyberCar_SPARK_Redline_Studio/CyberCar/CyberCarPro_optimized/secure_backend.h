// CYBER CAR SPARK — request validation, response hardening, and JSON-safety helpers.
// Kept separate from UI/actuator code so the security policy is reviewable in one file.
#pragma once
#include <Arduino.h>
#include <WebServer.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

namespace SecureBackend {
  constexpr char EXPECTED_ORIGIN[] = "http://192.168.4.1";

  inline void applySecurityHeaders(WebServer& server) {
    server.sendHeader("X-Frame-Options", "DENY");
    server.sendHeader("X-Content-Type-Options", "nosniff");
    server.sendHeader("Referrer-Policy", "no-referrer");
    server.sendHeader("Permissions-Policy", "camera=(), microphone=(), geolocation=()");
    server.sendHeader("Cache-Control", "no-store, max-age=0");
    server.sendHeader("Content-Security-Policy",
      "default-src 'self'; base-uri 'none'; object-src 'none'; frame-ancestors 'none'; "
      "form-action 'self'; connect-src 'self'; img-src 'self' data:; "
      "style-src 'self' 'unsafe-inline'; script-src 'self' 'unsafe-inline'");
  }

  inline void sendJson(WebServer& server, const int status, const char* constantOrSanitizedJson) {
    applySecurityHeaders(server);
    server.send(status, "application/json; charset=utf-8", constantOrSanitizedJson);
  }

  inline bool trustedOrigin(const String& origin) {
    return origin == EXPECTED_ORIGIN;
  }

  inline bool constantTimeEquals(const char* a, const size_t aLen,
                                 const char* b, const size_t bLen) {
    if (a == nullptr || b == nullptr || aLen != bLen) return false;
    uint8_t difference = 0;
    for (size_t i = 0; i < aLen; ++i) {
      difference |= static_cast<uint8_t>(a[i] ^ b[i]);
    }
    return difference == 0;
  }

  // Strict decimal parser: accepts digits and one leading minus only; no whitespace,
  // suffixes, exponent notation, overflow, or unbounded input is accepted.
  inline bool parseBoundedIntArg(WebServer& server, const char* name,
                                 const int minValue, const int maxValue, int& result) {
    if (!server.hasArg(name)) return false;
    const String text = server.arg(name);
    if (text.length() == 0 || text.length() > 8) return false;
    const char* p = text.c_str();
    if (*p == '-') ++p;
    if (*p == '\0') return false;
    for (; *p != '\0'; ++p) if (*p < '0' || *p > '9') return false;
    char* end = nullptr;
    const long value = strtol(text.c_str(), &end, 10);
    if (end == text.c_str() || *end != '\0' || value < minValue || value > maxValue) return false;
    result = static_cast<int>(value);
    return true;
  }

  inline bool allowedValue(const String& value, const char* a, const char* b = nullptr,
                           const char* c = nullptr) {
    return value == a || (b != nullptr && value == b) || (c != nullptr && value == c);
  }

  // Use this when a future API response needs to include a non-constant string.
  // Current telemetry only serializes fixed enum names and hard-coded literals.
  inline bool escapeJsonString(const char* input, char* output, const size_t capacity) {
    if (input == nullptr || output == nullptr || capacity < 3) return false;
    size_t out = 0;
    output[out++] = '"';
    for (const unsigned char* p = reinterpret_cast<const unsigned char*>(input); *p; ++p) {
      const unsigned char ch = *p;
      const char* replacement = nullptr;
      switch (ch) {
        case '"': replacement = "\\\""; break;
        case '\\': replacement = "\\\\"; break;
        case '\b': replacement = "\\b"; break;
        case '\f': replacement = "\\f"; break;
        case '\n': replacement = "\\n"; break;
        case '\r': replacement = "\\r"; break;
        case '\t': replacement = "\\t"; break;
        default: break;
      }
      if (replacement != nullptr) {
        const size_t n = strlen(replacement);
        if (out + n + 2 > capacity) return false;
        memcpy(output + out, replacement, n); out += n;
      } else if (ch < 0x20U) {
        if (out + 6U + 1U > capacity) return false;
        const int n = snprintf(output + out, capacity - out, "\\u%04x", static_cast<unsigned int>(ch));
        if (n != 6) return false;
        out += 6;
      } else {
        if (out + 2 > capacity) return false;
        output[out++] = static_cast<char>(ch);
      }
    }
    if (out + 2 > capacity) return false;
    output[out++] = '"';
    output[out] = '\0';
    return true;
  }
}
