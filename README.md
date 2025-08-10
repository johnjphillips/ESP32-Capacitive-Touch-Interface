# CapacitiveButton (ESP32 Arduino)

A small, self-contained helper for using ESP32 capacitive touch pins with:

- **EMA smoothing** for stable readings  
- **Hysteresis** with separate press/release thresholds  
- **Debounce**  
- **Short** and **long** press detection  
- Simple polling API (`update()`)

---

## 📦 Installation

### PlatformIO
1. Create `lib/CapacitiveButton/`.
2. Put `CapacitiveButton.h` and `CapacitiveButton.cpp` inside.
3. In your code:
   ```cpp
   #include "CapacitiveButton.h"
   ```

### Arduino IDE
1. Add both files to your sketch (same folder as `.ino`).
2. Include as above.

---

## 📌 Supported pins

ESP32 capacitive pins are **T0–T9**:

| Touch # | GPIO |
|:------:|:----:|
| T0 | 4 |
| T1 | 0 |
| T2 | 2 |
| T3 | 15 |
| T4 | 13 |
| T5 | 12 |
| T6 | 14 |
| T7 | 27 |
| T8 | 33 |
| T9 | 32 |

---

## ⚙️ Calibration — finding your thresholds

You need **two** numbers for each button:

- `pressThreshold` → when the smoothed value **falls below** this, a press starts.  
- `releaseThreshold` → when the smoothed value **rises above** this, a release occurs.

### Quick method

1. Print the smoothed value in `loop()`:
   ```cpp
   Serial.println(btn1.update());
   delay(50);
   ```
2. Note the average **no-touch** value.
3. Note the average **with-touch** value.
4. Choose:
   ```
   pressThreshold   = closer to the touch value (lower)
   releaseThreshold = closer to the no-touch value (higher)
   ```
   Leave a gap between them (hysteresis) for stability.

**Optional helper** (paste into your sketch to get quick numbers):
```cpp
struct TouchProfile { uint16_t idleAvg; uint16_t touchAvg; };

TouchProfile calibrateTouch(uint8_t tpin, uint16_t ms = 500) {
  auto sampleAvg = [&](uint16_t durationMs){
    uint32_t acc = 0, n = 0;
    unsigned long endAt = millis() + durationMs;
    while (millis() < endAt) { acc += touchRead(tpin); n++; delay(2); }
    return (uint16_t)(acc / (n ? n : 1));
  };

  Serial.printf("Calibrating T%u...\n", tpin);
  Serial.println("Keep finger OFF the pad...");
  delay(600);
  uint16_t idleAvg = sampleAvg(ms);

  Serial.println("Now TOUCH the pad...");
  delay(600);
  uint16_t touchAvg = sampleAvg(ms);

  Serial.printf("Idle=%u  Touch=%u  Gap=%d\n", idleAvg, touchAvg, (int)idleAvg - (int)touchAvg);
  return { idleAvg, touchAvg };
}

// Example usage in setup():
// auto p = calibrateTouch(T6);
// uint16_t pressThr   = p.idleAvg - (p.idleAvg - p.touchAvg) * 2 / 3; // ~closer to touch
// uint16_t releaseThr = p.idleAvg - (p.idleAvg - p.touchAvg) / 3;     // ~closer to idle
```

---

## 🚀 Quick start

```cpp
#include <Arduino.h>
#include "CapacitiveButton.h"

// pin, pressThreshold, releaseThreshold, [longPressMs=1000], [debounceMs=50]
CapacitiveButton btn1(T6, 65, 70);
CapacitiveButton btn2(T4, 60, 63);

void setup() {
  Serial.begin(115200);
}

void loop() {
  uint16_t val1 = btn1.update();

  if (btn1.isShortPressed()) Serial.println("btn1 short");
  if (btn1.isLongPressed())  Serial.println("btn1 long");

  // Debug current smoothed value
  // Serial.println(val1);

  delay(5);
}
```

---

## 📜 API reference

### Constructor
```cpp
CapacitiveButton(uint8_t touchPin,
                 uint16_t pressThreshold,
                 uint16_t releaseThreshold,
                 unsigned long longPressTime = 1000,
                 unsigned long debounceTime = 50);
```

### Methods
- `uint16_t update();`  
  Runs EMA smoothing + state machine; returns the current **smoothed** touch value.
- `bool isShortPressed();`  
  One-shot; returns `true` once when a short press is detected (then clears the flag).
- `bool isLongPressed();`  
  One-shot; returns `true` once when a long press is detected (then clears the flag).
- `ButtonState getState() const;`  
  Current state: `BUTTON_IDLE`, `BUTTON_PRESSED`, `BUTTON_HELD`, `BUTTON_RELEASED`.
- `uint16_t lastTouchValue() const;`  
  Last EMA value (without advancing logic).
- `void setEmaShift(uint8_t shift);`  
  Adjust EMA smoothing (α = 1 / 2^shift). Default `_emaShift` = **2** (α = **1/4**, fairly responsive).

---

## 🔍 How it works

1. **EMA smoothing** (integer, fast)
   ```cpp
   _ema = _ema - (_ema >> _emaShift) + (raw >> _emaShift);
   ```
   This is an exponential moving average implemented with shifts — smooth but cheap.

2. **State machine**
   - `IDLE → PRESSED`: value < `pressThreshold` and debounce passed.
   - `PRESSED → HELD`: press duration > `longPressTime`.
   - `PRESSED → RELEASED`: value > `releaseThreshold` (short press).
   - `HELD → RELEASED`: value > `releaseThreshold` (after long press).
   - `RELEASED → IDLE`: next update tick.

---

## 🎯 Tuning tips

- Keep `loop()` fast; call `update()` often for snappy behavior.  
- Ensure `pressThreshold < releaseThreshold` (hysteresis) to avoid chattering near the boundary.  
- Smoothing trade-offs:
  - Lower `_emaShift` → **more responsive**, less smooth.
  - Higher `_emaShift` → **smoother**, more lag.
