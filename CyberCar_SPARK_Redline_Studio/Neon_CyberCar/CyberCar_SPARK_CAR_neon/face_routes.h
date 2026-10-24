#ifndef SPARK_CAR_FACE_ROUTES_H
#define SPARK_CAR_FACE_ROUTES_H

#include <Arduino.h>
#include <WebServer.h>
#include <stddef.h>
#include "face_assets.h"
#include "secure_backend.h"

extern WebServer webServer;

// JPEG arrays are in flash (PROGMEM). send_P streams them without allocating a JPEG-sized heap buffer.
static void sendFaceAsset(const uint8_t* imageData, const size_t imageSize) {
  addSecurityHeaders(true); // same security headers; cache immutable art in the browser
  webServer.send_P(200, "image/jpeg", reinterpret_cast<PGM_P>(imageData), imageSize);
}

static void routeFaceAngry()    { sendFaceAsset(FaceAssets::kAngry, FaceAssets::kAngrySize); }
static void routeFaceAuto()     { sendFaceAsset(FaceAssets::kAuto, FaceAssets::kAutoSize); }
static void routeFaceBoot()     { sendFaceAsset(FaceAssets::kBoot, FaceAssets::kBootSize); }
static void routeFaceBrake()    { sendFaceAsset(FaceAssets::kBrake, FaceAssets::kBrakeSize); }
static void routeFaceDance()    { sendFaceAsset(FaceAssets::kDance, FaceAssets::kDanceSize); }
static void routeFaceDance2()   { sendFaceAsset(FaceAssets::kDance2, FaceAssets::kDance2Size); }
static void routeFaceDrift()    { sendFaceAsset(FaceAssets::kDrift, FaceAssets::kDriftSize); }
static void routeFaceHappy()    { sendFaceAsset(FaceAssets::kHappy, FaceAssets::kHappySize); }
static void routeFaceIdle()     { sendFaceAsset(FaceAssets::kIdle, FaceAssets::kIdleSize); }
static void routeFaceManual()   { sendFaceAsset(FaceAssets::kManual, FaceAssets::kManualSize); }
static void routeFaceQlearning(){ sendFaceAsset(FaceAssets::kQlearning, FaceAssets::kQlearningSize); }

static void registerFaceAssetRoutes() {
  webServer.on("/assets/faces/angry.jpg", HTTP_GET, routeFaceAngry);
  webServer.on("/assets/faces/auto.jpg", HTTP_GET, routeFaceAuto);
  webServer.on("/assets/faces/boot.jpg", HTTP_GET, routeFaceBoot);
  webServer.on("/assets/faces/brake.jpg", HTTP_GET, routeFaceBrake);
  webServer.on("/assets/faces/dance.jpg", HTTP_GET, routeFaceDance);
  webServer.on("/assets/faces/dance2.jpg", HTTP_GET, routeFaceDance2);
  webServer.on("/assets/faces/drift.jpg", HTTP_GET, routeFaceDrift);
  webServer.on("/assets/faces/happy.jpg", HTTP_GET, routeFaceHappy);
  webServer.on("/assets/faces/idle.jpg", HTTP_GET, routeFaceIdle);
  webServer.on("/assets/faces/manual.jpg", HTTP_GET, routeFaceManual);
  webServer.on("/assets/faces/qlearning.jpg", HTTP_GET, routeFaceQlearning);
}

#endif // SPARK_CAR_FACE_ROUTES_H
