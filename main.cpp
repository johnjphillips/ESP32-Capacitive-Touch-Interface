#include <Arduino.h>
#include "CapacitiveButton.h"


// Define buttons
CapacitiveButton btn1(T6, 65, 70); // touch pin, press threshold, release threshold
CapacitiveButton btn2(T4, 60, 63);
CapacitiveButton btn3(T9, 72, 80);
CapacitiveButton btn4(T8, 72, 80);

void setup() {
  Serial.begin(115200);
  delay(1000); // give me time to bring up serial monitor
  Serial.println("ESP32 Touch Test");
}

void loop() {

  btn1.update();
  btn2.update();
  btn3.update();
  btn4.update();

  if (btn1.isShortPressed()) {
    Serial.println("btn1 Short Press");
  }

  if(btn1.isLongPressed()) {
    Serial.println("btn1 Long Press");
  }


  if (btn2.isShortPressed()) {
    Serial.println("btn2 Short Press");
  }

  if(btn2.isLongPressed()) {
    Serial.println("btn2 Long Press");
  }


  if (btn3.isShortPressed()) {
    Serial.println("btn3 Short Press");
  }

  if(btn3.isLongPressed()) {
    Serial.println("btn3 Long Press");
  }


  if (btn4.isShortPressed()) {
    Serial.println("btn4 Short Press");
  }

  if(btn4.isLongPressed()) {
    Serial.println("btn4 Long Press");
  }


  // Output the reads to Serial so thew threshold values can be determined and set

  //Serial.printf("| %d | %d | %d | %d | \n", btn1.update(), btn2.update(), btn3.update(), btn4.update());
  //delay(50);

  


  }
  
