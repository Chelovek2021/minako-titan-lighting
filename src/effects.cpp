#include "effects.h"

uint32_t Effects::getRainbowColor(uint8_t pos) {
  return colorWheel(pos);
}

uint8_t Effects::calculatePulse(unsigned long now, uint16_t speed, uint8_t maxBright) {
  uint16_t phase = (now / speed) % 512;
  
  if (phase < 256) {
    return (phase * maxBright) / 256;
  } else {
    return ((512 - phase) * maxBright) / 256;
  }
}

uint8_t Effects::calculateChase(unsigned long now, uint16_t speed, uint8_t maxBright) {
  uint8_t chase = (now / (speed / 10)) % 20;
  
  if (chase < 10) {
    return (chase * maxBright) / 10;
  } else {
    return ((20 - chase) * maxBright) / 10;
  }
}

uint8_t Effects::calculateStrobe(unsigned long now, uint16_t speed) {
  if ((now / (speed / 10)) % 2 == 0) {
    return 255;
  }
  return 0;
}

uint8_t Effects::calculateBlink(unsigned long now, uint16_t speed, uint8_t maxBright) {
  if ((now / speed) % 2 == 0) {
    return maxBright;
  }
  return 0;
}

uint32_t Effects::colorWheel(uint8_t pos) {
  pos = 255 - pos;
  
  if (pos < 85) {
    return ((255 - pos * 3) << 16) | (0 << 8) | (pos * 3);
  }
  
  if (pos < 170) {
    pos -= 85;
    return (0 << 16) | ((pos * 3) << 8) | (255 - pos * 3);
  }
  
  pos -= 170;
  return ((pos * 3) << 16) | (255 - pos * 3) | (0 << 8);
}