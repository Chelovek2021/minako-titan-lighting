#include <Arduino.h>
#include <WiFi.h>
#include <AsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <Ticker.h>
#include "lighting_manager.h"
#include "web_interface.h"
#include "effects.h"

// WiFi credentials
const char* ssid = "MinakoTitan_Light";
const char* password = "12345678";

// Pin definitions (ВРЕМЕННЫЕ - замени на свои)
#define PIN_FRONT_LEFT_TURN 12
#define PIN_FRONT_RIGHT_TURN 13
#define PIN_CENTER_HEADLIGHT 14
#define PIN_LEFT_HEADLIGHT 15
#define PIN_RIGHT_HEADLIGHT 2

#define PIN_REAR_BRAKE 4
#define PIN_REAR_PARKING 5

#define PIN_SIDE_LEFT_STRIP 25
#define PIN_SIDE_RIGHT_STRIP 26

// PWM Settings
#define PWM_FREQUENCY 5000
#define PWM_RESOLUTION 8

AsyncWebServer server(80);
LightingManager lightManager;
WebInterface webInterface;
Ticker updateTicker;

void setupWiFi() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n=== MinakoTitan Lighting System ===");
  Serial.println("Starting WiFi Access Point...");
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("📡 AP IP address: ");
  Serial.println(IP);
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(password);
}

void setupPins() {
  Serial.println("Configuring pins...");
  
  // Front lights
  pinMode(PIN_FRONT_LEFT_TURN, OUTPUT);
  pinMode(PIN_FRONT_RIGHT_TURN, OUTPUT);
  pinMode(PIN_CENTER_HEADLIGHT, OUTPUT);
  pinMode(PIN_LEFT_HEADLIGHT, OUTPUT);
  pinMode(PIN_RIGHT_HEADLIGHT, OUTPUT);
  
  // Rear lights
  pinMode(PIN_REAR_BRAKE, OUTPUT);
  pinMode(PIN_REAR_PARKING, OUTPUT);
  
  // Side strips (PWM for effects)
  pinMode(PIN_SIDE_LEFT_STRIP, OUTPUT);
  pinMode(PIN_SIDE_RIGHT_STRIP, OUTPUT);
  
  // Setup PWM channels
  ledcSetup(0, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcSetup(1, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcSetup(2, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcSetup(3, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcSetup(4, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcSetup(5, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcSetup(6, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcSetup(7, PWM_FREQUENCY, PWM_RESOLUTION);
  
  ledcAttachPin(PIN_FRONT_LEFT_TURN, 0);
  ledcAttachPin(PIN_FRONT_RIGHT_TURN, 1);
  ledcAttachPin(PIN_CENTER_HEADLIGHT, 2);
  ledcAttachPin(PIN_LEFT_HEADLIGHT, 3);
  ledcAttachPin(PIN_RIGHT_HEADLIGHT, 4);
  ledcAttachPin(PIN_REAR_BRAKE, 5);
  ledcAttachPin(PIN_REAR_PARKING, 6);
  ledcAttachPin(PIN_SIDE_LEFT_STRIP, 7);
  
  Serial.println("✓ All pins configured");
}

void setupSPIFFS() {
  if(!SPIFFS.begin(true)){
    Serial.println("✗ SPIFFS Mount Failed");
    return;
  }
  Serial.println("✓ SPIFFS mounted successfully");
}

void setupLightingManager() {
  LightingManager::PinConfig pinConfig = {
    .front_left_turn = PIN_FRONT_LEFT_TURN,
    .front_right_turn = PIN_FRONT_RIGHT_TURN,
    .center_headlight = PIN_CENTER_HEADLIGHT,
    .left_headlight = PIN_LEFT_HEADLIGHT,
    .right_headlight = PIN_RIGHT_HEADLIGHT,
    .rear_brake = PIN_REAR_BRAKE,
    .rear_parking = PIN_REAR_PARKING,
    .side_left_strip = PIN_SIDE_LEFT_STRIP,
    .side_right_strip = PIN_SIDE_RIGHT_STRIP
  };
  
  lightManager.init(pinConfig);
  Serial.println("✓ Lighting Manager initialized");
}

void onUpdate() {
  lightManager.update();
}

void setup() {
  setupWiFi();
  setupPins();
  setupSPIFFS();
  setupLightingManager();
  
  webInterface.setupRoutes(server, lightManager);
  
  server.begin();
  Serial.println("✓ Web Server started");
  Serial.println("🚀 Ready to go!");
  
  // Update ticker for light effects
  updateTicker.attach_ms(20, onUpdate);
}

void loop() {
  // Main loop - everything is handled by async tasks
  delay(100);
}