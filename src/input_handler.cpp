#include "input_handler.h"

void InputHandler::init(InputConfig config) {
  pins_ = config;
  
  pinMode(pins_.left_turn_input, INPUT_PULLUP);
  pinMode(pins_.right_turn_input, INPUT_PULLUP);
  pinMode(pins_.brake_input, INPUT_PULLUP);
  pinMode(pins_.parking_input, INPUT_PULLUP);
  pinMode(pins_.mode_button_input, INPUT_PULLUP);
  pinMode(pins_.brightness_up_input, INPUT_PULLUP);
  pinMode(pins_.brightness_down_input, INPUT_PULLUP);
  
  Serial.println("✓ Input handler initialized");
}

void InputHandler::update() {
  unsigned long now = millis();
  
  if (now - last_debounce_time_ < DEBOUNCE_DELAY) {
    return;
  }
  last_debounce_time_ = now;
  
  checkInput(pins_.left_turn_input, left_turn_state_, left_turn_last_, LEFT_TURN);
  checkInput(pins_.right_turn_input, right_turn_state_, right_turn_last_, RIGHT_TURN);
  checkInput(pins_.brake_input, brake_state_, brake_last_, BRAKE);
  checkInput(pins_.parking_input, parking_state_, parking_last_, PARKING);
  checkInput(pins_.mode_button_input, mode_button_state_, mode_button_last_, MODE_BUTTON);
  
  bool brightness_up = readInputPin(pins_.brightness_up_input);
  bool brightness_down = readInputPin(pins_.brightness_down_input);
  
  if (brightness_up && !brightness_up_last_) {
    if (callback_) callback_(BRIGHTNESS_UP, true);
  }
  brightness_up_last_ = brightness_up;
  
  if (brightness_down && !brightness_down_last_) {
    if (callback_) callback_(BRIGHTNESS_DOWN, true);
  }
  brightness_down_last_ = brightness_down;
}

void InputHandler::checkInput(uint8_t pin, bool& current_state, bool& last_state, InputType type) {
  bool pin_state = readInputPin(pin);
  
  if (pin_state != last_state) {
    current_state = pin_state;
    if (callback_) {
      callback_(type, pin_state);
    }
  }
  last_state = pin_state;
}

bool InputHandler::readInputPin(uint8_t pin) {
  return digitalRead(pin) == LOW;
}
