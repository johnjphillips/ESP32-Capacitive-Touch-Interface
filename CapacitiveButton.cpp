#include "CapacitiveButton.h"

CapacitiveButton::CapacitiveButton(uint8_t touchPin, uint16_t pressThreshold, uint16_t releaseThreshold, unsigned long longPressTime, unsigned long debounceTime)
  : _touchPin(touchPin),
    _pressThreshold(pressThreshold),
    _releaseThreshold(releaseThreshold),
    _longPressTime(longPressTime),
    _debounceTime(debounceTime),
    _shortPressDetected(false),
    _longPressDetected(false),
    _lastTouchTime(0),
    _pressStartTime(0),
    _state(BUTTON_IDLE) {
  touchAttachInterrupt(_touchPin, NULL, 0); // Disable interrupt, use polling
}

uint16_t CapacitiveButton::update() {
  // --- Read & smooth ---
  uint16_t raw = touchRead(_touchPin);
  if (!_emaInit) { _ema = raw; _emaInit = true; }
  _ema = _ema - (_ema >> _emaShift) + (raw >> _emaShift); // integer EMA
  uint16_t touchValue = _ema;
  _lastTouchValue = touchValue;

  unsigned long now = millis();

  // --- State machine ---
  switch (_state) {
    case BUTTON_IDLE:
      if (touchValue < _pressThreshold && (now - _lastTouchTime) > _debounceTime) {
        _state = BUTTON_PRESSED;
        _pressStartTime = now;
        _lastTouchTime = now;
      }
      break;

    case BUTTON_PRESSED:
      if (touchValue < _pressThreshold) {
        if ((now - _pressStartTime) > _longPressTime) {
          _longPressDetected = true;
          _state = BUTTON_HELD;
        }
      } else if (touchValue > _releaseThreshold && (now - _lastTouchTime) > _debounceTime) {
        _shortPressDetected = true;
        _state = BUTTON_RELEASED;
        _lastTouchTime = now;
      }
      break;

    case BUTTON_HELD:
      if (touchValue > _releaseThreshold && (now - _lastTouchTime) > _debounceTime) {
        _state = BUTTON_RELEASED;
        _lastTouchTime = now;
      }
      break;

    case BUTTON_RELEASED:
      _state = BUTTON_IDLE;
      break;
  }

  return touchValue;
}


bool CapacitiveButton::isShortPressed() {
  if (_shortPressDetected) {
    _shortPressDetected = false;
    return true;
  }
  return false;
}

bool CapacitiveButton::isLongPressed() {
  if (_longPressDetected) {
    _longPressDetected = false;
    return true;
  }
  return false;
}


/*
ButtonState CapacitiveButton::getState() {
  return _state;
}


*/
