// CapacitiveButton.h
#ifndef CAPACITIVE_BUTTON_H
#define CAPACITIVE_BUTTON_H

#include <Arduino.h>

enum ButtonState : uint8_t {
  BUTTON_IDLE = 0,
  BUTTON_PRESSED,
  BUTTON_HELD,
  BUTTON_RELEASED
};

class CapacitiveButton {
public:
  CapacitiveButton(uint8_t touchPin,
                   uint16_t pressThreshold,
                   uint16_t releaseThreshold,
                   unsigned long longPressTime = 1000,
                   unsigned long debounceTime = 50);

  // Updates state machine and returns the current *smoothed* touch value
  uint16_t update();

  bool isShortPressed();
  bool isLongPressed();
  ButtonState getState() const { return _state; }

  // Optional helpers
  uint16_t lastTouchValue() const { return _lastTouchValue; }   // last EMA value
  void setEmaShift(uint8_t shift) { _emaShift = shift; }        // α = 1 / 2^shift

private:
  // Config
  uint8_t  _touchPin;
  uint16_t _pressThreshold;
  uint16_t _releaseThreshold;
  unsigned long _longPressTime;
  unsigned long _debounceTime;

  // Runtime flags/state
  bool _shortPressDetected;
  bool _longPressDetected;
  unsigned long _lastTouchTime;
  unsigned long _pressStartTime;
  ButtonState _state;

  // EMA smoothing (integer EMA: α = 1 / 2^_emaShift)
  uint16_t _ema = 0;
  bool _emaInit = false;
  uint8_t _emaShift = 2; // default α = 1/8 (tweakable)

  // For external access/telemetry
  uint16_t _lastTouchValue = 0;
};

#endif
