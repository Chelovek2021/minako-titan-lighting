#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <Arduino.h>
#include <functional>

class InputHandler {
public:
  enum InputType {
    LEFT_TURN = 0,
    RIGHT_TURN = 1,
    BRAKE = 2,
    PARKING = 3,
    MODE_BUTTON = 4,
    BRIGHTNESS_UP = 5,
    BRIGHTNESS_DOWN = 6
  };

  struct InputConfig {
    uint8_t left_turn_input;
    uint8_t right_turn_input;
    uint8_t brake_input;
    uint8_t parking_input;
    uint8_t mode_button_input;
    uint8_t brightness_up_input;
    uint8_t brightness_down_input;
  };

  typedef std::function<void(InputType, bool)> InputCallback;

  void init(InputConfig config);
  void update();
  void setCallback(InputCallback callback) { callback_ = callback; }
  
  bool getLeftTurnState() const { return left_turn_state_; }
  bool getRightTurnState() const { return right_turn_state_; }
  bool getBrakeState() const { return brake_state_; }
  bool getParkingState() const { return parking_state_; }

private:
  InputConfig pins_;
  InputCallback callback_;
  
  bool left_turn_state_ = false;
  bool right_turn_state_ = false;
  bool brake_state_ = false;
  bool parking_state_ = false;
  bool mode_button_state_ = false;
  
  unsigned long last_debounce_time_ = 0;
  const unsigned long DEBOUNCE_DELAY = 50;
  
  bool left_turn_last_ = false;
  bool right_turn_last_ = false;
  bool brake_last_ = false;
  bool parking_last_ = false;
  bool mode_button_last_ = false;
  bool brightness_up_last_ = false;
  bool brightness_down_last_ = false;
  
  void checkInput(uint8_t pin, bool& current_state, bool& last_state, InputType type);
  bool readInputPin(uint8_t pin);
};

#endif
