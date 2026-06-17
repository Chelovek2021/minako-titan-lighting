#ifndef EFFECTS_H
#define EFFECTS_H

#include <Arduino.h>

class Effects {
public:
  // Rainbow effect
  static uint32_t getRainbowColor(uint8_t pos);
  
  // Pulse calculation
  static uint8_t calculatePulse(unsigned long now, uint16_t speed, uint8_t maxBright);
  
  // Chase effect
  static uint8_t calculateChase(unsigned long now, uint16_t speed, uint8_t maxBright);
  
  // Strobe effect
  static uint8_t calculateStrobe(unsigned long now, uint16_t speed);
  
  // Blink effect
  static uint8_t calculateBlink(unsigned long now, uint16_t speed, uint8_t maxBright);
  
private:
  // Color wheel for rainbow
  static uint32_t colorWheel(uint8_t pos);
};

#endif // EFFECTS_H