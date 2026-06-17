#include "lighting_manager.h"
#include <SPIFFS.h>
#include <ArduinoJson.h>

#define PWM_RESOLUTION_MAX 255
#define LED_CHANNEL_OFFSET 0

void LightingManager::init(PinConfig config) {
  pins = config;
  loadSettings();
}

void LightingManager::update() {
  unsigned long now = millis();
  
  updateSideStripEffect();
  updateTurnSignals();
  updateEscortMode();
}

void LightingManager::setHeadlight(uint8_t brightness) {
  state.brightness = brightness;
  setLedBrightness(2, brightness); // CENTER_HEADLIGHT
  setLedBrightness(3, brightness); // LEFT_HEADLIGHT
  setLedBrightness(4, brightness); // RIGHT_HEADLIGHT
}

void LightingManager::setLeftHeadlight(uint8_t brightness) {
  setLedBrightness(3, brightness);
}

void LightingManager::setRightHeadlight(uint8_t brightness) {
  setLedBrightness(4, brightness);
}

void LightingManager::setCenterHeadlight(uint8_t brightness) {
  setLedBrightness(2, brightness);
}

void LightingManager::setLeftTurn(bool active, uint8_t brightness) {
  state.left_turn = active;
  if (active) {
    setLedBrightness(0, brightness);
  } else {
    setLedBrightness(0, 0);
  }
}

void LightingManager::setRightTurn(bool active, uint8_t brightness) {
  state.right_turn = active;
  if (active) {
    setLedBrightness(1, brightness);
  } else {
    setLedBrightness(1, 0);
  }
}

void LightingManager::setBrake(bool active, uint8_t brightness) {
  state.brake = active;
  if (active) {
    setLedBrightness(5, brightness);
  } else {
    setLedBrightness(5, 0);
  }
}

void LightingManager::setParking(bool active, uint8_t brightness) {
  state.parking = active;
  if (active) {
    setLedBrightness(6, brightness);
  } else {
    setLedBrightness(6, 0);
  }
}

void LightingManager::setSideStripMode(LightMode mode) {
  state.mode = mode;
}

void LightingManager::setSideStripBrightness(uint8_t brightness) {
  state.brightness = brightness;
}

void LightingManager::setSideStripSpeed(uint16_t speed_ms) {
  state.speed = speed_ms;
}

void LightingManager::setSideStripColor(uint32_t color) {
  state.color = color;
}

void LightingManager::setEscortMode(bool enabled, uint8_t duration_sec) {
  escortActive = enabled;
  escortDuration = duration_sec;
  if (enabled) {
    escortStartTime = millis();
    // Включаем фары
    setHeadlight(255);
  }
}

void LightingManager::setMusicReactive(bool enabled) {
  if (enabled) {
    state.mode = MUSIC_REACTIVE;
  }
}

void LightingManager::updateAudioLevel(uint8_t level) {
  audioLevel = level;
}

void LightingManager::allOff() {
  for (int i = 0; i < 8; i++) {
    ledcWrite(i, 0);
  }
  state.enabled = false;
}

void LightingManager::allOn(uint8_t brightness) {
  setHeadlight(brightness);
  setLedBrightness(5, brightness); // Brake
  setLedBrightness(6, brightness); // Parking
  state.enabled = true;
}

void LightingManager::updateSideStripEffect() {
  unsigned long now = millis();
  
  if (!state.enabled) return;
  
  effectTimer = (now / state.speed) % 256;
  
  switch (state.mode) {
    case STATIC:
      ledcWrite(7, state.brightness);
      break;
      
    case BLINK:
      if ((now / state.speed) % 2 == 0) {
        ledcWrite(7, state.brightness);
      } else {
        ledcWrite(7, 0);
      }
      break;
      
    case PULSE:
      ledcWrite(7, calculatePulse(state.speed));
      break;
      
    case STROBE:
      if ((now / (state.speed / 10)) % 2 == 0) {
        ledcWrite(7, 255);
      } else {
        ledcWrite(7, 0);
      }
      break;
      
    case CHASE:
      // Chase effect - brightness increases and decreases
      {
        uint8_t chase = (now / (state.speed / 10)) % 20;
        if (chase < 10) {
          ledcWrite(7, (chase * state.brightness) / 10);
        } else {
          ledcWrite(7, ((20 - chase) * state.brightness) / 10);
        }
      }
      break;
      
    case RAINBOW:
      // Rainbow cycle effect
      {
        uint8_t hue = (now / state.speed) % 360;
        uint8_t brightness = state.brightness;
        // For simplicity - cycling through RGB
        if (hue < 120) {
          ledcWrite(7, (hue * brightness) / 120);
        } else if (hue < 240) {
          ledcWrite(7, brightness - ((hue - 120) * brightness) / 120);
        } else {
          ledcWrite(7, ((hue - 240) * brightness) / 120);
        }
      }
      break;
      
    case MUSIC_REACTIVE:
      // React to audio level
      ledcWrite(7, audioLevel);
      break;
      
    default:
      ledcWrite(7, 0);
  }
}

void LightingManager::updateTurnSignals() {
  unsigned long now = millis();
  
  if (state.left_turn || state.right_turn) {
    // Blink pattern for turn signals
    if ((now / 500) % 2 == 0) {
      if (state.left_turn) setLedBrightness(0, 255);
      if (state.right_turn) setLedBrightness(1, 255);
    } else {
      if (state.left_turn) setLedBrightness(0, 0);
      if (state.right_turn) setLedBrightness(1, 0);
    }
  }
}

void LightingManager::updateEscortMode() {
  if (!escortActive) return;
  
  unsigned long elapsed = (millis() - escortStartTime) / 1000;
  
  if (elapsed >= escortDuration) {
    escortActive = false;
    setHeadlight(0);
  }
}

uint8_t LightingManager::calculatePulse(uint16_t speed) {
  unsigned long now = millis();
  uint8_t phase = (now / (speed / 256)) % 512;
  
  if (phase < 256) {
    return (phase * state.brightness) / 256;
  } else {
    return ((512 - phase) * state.brightness) / 256;
  }
}

uint32_t LightingManager::wheelColor(uint8_t pos) {
  pos = 255 - pos;
  if(pos < 85) {
    return ((255 - pos * 3) << 16) | (0 << 8) | (pos * 3);
  }
  if(pos < 170) {
    pos -= 85;
    return (0 << 16) | ((pos * 3) << 8) | (255 - pos * 3);
  }
  pos -= 170;
  return ((pos * 3) << 16) | (255 - pos * 3) | (0 << 8);
}

void LightingManager::setLedBrightness(uint8_t channel, uint8_t brightness) {
  ledcWrite(channel, brightness);
}

void LightingManager::saveSettings() {
  DynamicJsonDocument doc(1024);
  
  doc["mode"] = state.mode;
  doc["brightness"] = state.brightness;
  doc["speed"] = state.speed;
  doc["color"] = state.color;
  doc["enabled"] = state.enabled;
  
  File file = SPIFFS.open("/settings.json", "w");
  if (file) {
    serializeJson(doc, file);
    file.close();
    Serial.println("✓ Settings saved");
  }
}

void LightingManager::loadSettings() {
  if (SPIFFS.exists("/settings.json")) {
    File file = SPIFFS.open("/settings.json", "r");
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, file);
    
    state.mode = (LightMode)(int)doc["mode"];
    state.brightness = doc["brightness"];
    state.speed = doc["speed"];
    state.color = doc["color"];
    state.enabled = doc["enabled"];
    
    file.close();
    Serial.println("✓ Settings loaded");
  } else {
    Serial.println("⚠ No settings found, using defaults");
  }
}