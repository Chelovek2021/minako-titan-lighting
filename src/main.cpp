#include <Arduino.h>
#include <WiFi.h>
#include <AsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <Ticker.h>
#include "lighting_manager.h"
#include "input_handler.h"
#include "web_interface.h"
#include "effects.h"

const char* ssid = "MinakoTitan_Light";
const char* password = "12345678";

#define PIN_FRONT_LEFT_TURN 12
#define PIN_FRONT_RIGHT_TURN 13
#define PIN_CENTER_HEADLIGHT 14
#define PIN_LEFT_HEADLIGHT 15
#define PIN_RIGHT_HEADLIGHT 2
#define PIN_REAR_BRAKE 4
#define PIN_REAR_PARKING 5
#define PIN_SIDE_LEFT_STRIP 25
#define PIN_SIDE_RIGHT_STRIP 26

#define PIN_INPUT_LEFT_TURN 32
#define PIN_INPUT_RIGHT_TURN 33
#define PIN_INPUT_BRAKE 34
#define PIN_INPUT_PARKING 35
#define PIN_INPUT_MODE 36
#define PIN_INPUT_BRIGHTNESS_UP 39
#define PIN_INPUT_BRIGHTNESS_DOWN 37

#define PWM_FREQUENCY 5000
#define PWM_RESOLUTION 8

AsyncWebServer server(80);
LightingManager lightManager;
InputHandler inputHandler;
WebInterface webInterface;
Ticker updateTicker;

void handleInputEvent(InputHandler::InputType type, bool state);
void cycleEffectMode();
void increaseBrightness();
void decreaseBrightness();

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

void setupOutputPins() {
  Serial.println("\n🔌 Configuring output pins...");
  
  pinMode(PIN_FRONT_LEFT_TURN, OUTPUT);
  pinMode(PIN_FRONT_RIGHT_TURN, OUTPUT);
  pinMode(PIN_CENTER_HEADLIGHT, OUTPUT);
  pinMode(PIN_LEFT_HEADLIGHT, OUTPUT);
  pinMode(PIN_RIGHT_HEADLIGHT, OUTPUT);
  pinMode(PIN_REAR_BRAKE, OUTPUT);
  pinMode(PIN_REAR_PARKING, OUTPUT);
  pinMode(PIN_SIDE_LEFT_STRIP, OUTPUT);
  pinMode(PIN_SIDE_RIGHT_STRIP, OUTPUT);
  
  for (int i = 0; i < 8; i++) {
    ledcSetup(i, PWM_FREQUENCY, PWM_RESOLUTION);
  }
  
  ledcAttachPin(PIN_FRONT_LEFT_TURN, 0);
  ledcAttachPin(PIN_FRONT_RIGHT_TURN, 1);
  ledcAttachPin(PIN_CENTER_HEADLIGHT, 2);
  ledcAttachPin(PIN_LEFT_HEADLIGHT, 3);
  ledcAttachPin(PIN_RIGHT_HEADLIGHT, 4);
  ledcAttachPin(PIN_REAR_BRAKE, 5);
  ledcAttachPin(PIN_REAR_PARKING, 6);
  ledcAttachPin(PIN_SIDE_LEFT_STRIP, 7);
  
  Serial.println("✓ Output pins configured");
}

void setupInputPins() {
  Serial.println("\n📱 Configuring input pins...");
  
  InputHandler::InputConfig inputConfig = {
    .left_turn_input = PIN_INPUT_LEFT_TURN,
    .right_turn_input = PIN_INPUT_RIGHT_TURN,
    .brake_input = PIN_INPUT_BRAKE,
    .parking_input = PIN_INPUT_PARKING,
    .mode_button_input = PIN_INPUT_MODE,
    .brightness_up_input = PIN_INPUT_BRIGHTNESS_UP,
    .brightness_down_input = PIN_INPUT_BRIGHTNESS_DOWN
  };
  
  inputHandler.init(inputConfig);
  inputHandler.setCallback([](InputHandler::InputType type, bool state) {
    handleInputEvent(type, state);
  });
  
  Serial.println("✓ Input pins configured");
}

void handleInputEvent(InputHandler::InputType type, bool state) {
  switch (type) {
    case InputHandler::LEFT_TURN:
      Serial.printf("🔄 Left Turn: %s\n", state ? "PRESSED" : "RELEASED");
      lightManager.setLeftTurn(state, 255);
      break;
      
    case InputHandler::RIGHT_TURN:
      Serial.printf("🔄 Right Turn: %s\n", state ? "PRESSED" : "RELEASED");
      lightManager.setRightTurn(state, 255);
      break;
      
    case InputHandler::BRAKE:
      Serial.printf("🛑 Brake: %s\n", state ? "ACTIVE" : "INACTIVE");
      lightManager.setBrake(state, 255);
      break;
      
    case InputHandler::PARKING:
      Serial.printf("💡 Parking: %s\n", state ? "ACTIVE" : "INACTIVE");
      lightManager.setParking(state, 200);
      break;
      
    case InputHandler::MODE_BUTTON:
      if (state) {
        Serial.println("🎨 Mode Button Pressed");
        cycleEffectMode();
      }
      break;
      
    case InputHandler::BRIGHTNESS_UP:
      if (state) {
        Serial.println("📈 Brightness Up");
        increaseBrightness();
      }
      break;
      
    case InputHandler::BRIGHTNESS_DOWN:
      if (state) {
        Serial.println("📉 Brightness Down");
        decreaseBrightness();
      }
      break;
  }
}

void cycleEffectMode() {
  static int currentMode = 1;
  currentMode = (currentMode + 1) % 8;
  lightManager.setSideStripMode((LightingManager::LightMode)currentMode);
  Serial.printf("Effect Mode: %d\n", currentMode);
}

void increaseBrightness() {
  LightingManager::LightState state = lightManager.getState();
  uint8_t newBrightness = state.brightness;
  
  if (newBrightness < 255 - 15) {
    newBrightness += 15;
  } else {
    newBrightness = 255;
  }
  
  lightManager.setSideStripBrightness(newBrightness);
  Serial.printf("Brightness: %d\n", newBrightness);
}

void decreaseBrightness() {
  LightingManager::LightState state = lightManager.getState();
  uint8_t newBrightness = state.brightness;
  
  if (newBrightness > 15) {
    newBrightness -= 15;
  } else {
    newBrightness = 0;
  }
  
  lightManager.setSideStripBrightness(newBrightness);
  Serial.printf("Brightness: %d\n", newBrightness);
}

void setupSPIFFS() {
  if(!SPIFFS.begin(true)){
    Serial.println("✗ SPIFFS Mount Failed");
    return;
  }
  Serial.println("✓ SPIFFS mounted");
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
  inputHandler.update();
}

void setup() {
  setupWiFi();
  setupOutputPins();
  setupSPIFFS();
  setupLightingManager();
  setupInputPins();
  
  webInterface.setupRoutes(server, lightManager);
  
  server.begin();
  Serial.println("\n✓ Web Server started");
  Serial.println("🚀 Ready to go!\n");
  
  updateTicker.attach_ms(20, onUpdate);
}

void loop() {
  delay(100);
}
