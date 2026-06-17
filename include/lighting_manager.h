#ifndef LIGHTING_MANAGER_H
#define LIGHTING_MANAGER_H

#include <Arduino.h>
#include <vector>

class LightingManager {
public:
  struct PinConfig {
    uint8_t front_left_turn;
    uint8_t front_right_turn;
    uint8_t center_headlight;
    uint8_t left_headlight;
    uint8_t right_headlight;
    uint8_t rear_brake;
    uint8_t rear_parking;
    uint8_t side_left_strip;
    uint8_t side_right_strip;
  };

  enum LightMode {
    OFF = 0,
    STATIC = 1,
    BLINK = 2,
    PULSE = 3,
    CHASE = 4,
    MUSIC_REACTIVE = 5,
    RAINBOW = 6,
    STROBE = 7
  };

  struct LightState {
    LightMode mode = STATIC;
    uint8_t brightness = 255;
    uint16_t speed = 1000; // ms
    uint32_t color = 0xFFFFFF; // RGB
    bool enabled = true;
    bool left_turn = false;
    bool right_turn = false;
    bool brake = false;
    bool parking = false;
  };

  void init(PinConfig config);
  void update();
  
  // Front lights control
  void setHeadlight(uint8_t brightness);
  void setLeftHeadlight(uint8_t brightness);
  void setRightHeadlight(uint8_t brightness);
  void setCenterHeadlight(uint8_t brightness);
  
  // Turn signals
  void setLeftTurn(bool active, uint8_t brightness = 255);
  void setRightTurn(bool active, uint8_t brightness = 255);
  
  // Rear lights
  void setBrake(bool active, uint8_t brightness = 255);
  void setParking(bool active, uint8_t brightness = 255);
  
  // Side strips effects
  void setSideStripMode(LightMode mode);
  void setSideStripBrightness(uint8_t brightness);
  void setSideStripSpeed(uint16_t speed_ms);
  void setSideStripColor(uint32_t color);
  
  // Escort mode (follow light) - when turning off
  void setEscortMode(bool enabled, uint8_t duration_sec);
  
  // Music reactive mode
  void setMusicReactive(bool enabled);
  void updateAudioLevel(uint8_t level);
  
  // Global state
  LightState getState() const { return state; }
  void setState(const LightState& newState) { state = newState; }
  void allOff();
  void allOn(uint8_t brightness = 255);
  void saveSettings();
  void loadSettings();

private:
  PinConfig pins;
  LightState state;
  
  // Timing variables
  unsigned long lastUpdateTime = 0;
  unsigned long effectTimer = 0;
  uint8_t audioLevel = 0;
  bool escortActive = false;
  unsigned long escortStartTime = 0;
  uint16_t escortDuration = 120; // seconds
  
  // Helper functions
  void updateSideStripEffect();
  void updateTurnSignals();
  void updateEscortMode();
  uint8_t calculatePulse(uint16_t speed);
  uint32_t wheelColor(uint8_t pos);
  void setLedBrightness(uint8_t channel, uint8_t brightness);
};

#endif // LIGHTING_MANAGER_H