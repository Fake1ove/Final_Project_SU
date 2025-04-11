#include "Arduino.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// กำหนดที่อยู่ I2C ของ LCD และขนาด 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

String receivedDataESP = "";

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  
  lcd.setCursor(0, 0);
  lcd.print("Siri Test");
}

void loop() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      receivedDataESP.trim();

      lcd.clear(); // เคลียร์จอทุกครั้งก่อนแสดงข้อความใหม่
      lcd.setCursor(0, 0);

      if (receivedDataESP == "M1_ON") {
        lcd.print("Siri M1_ON");

      } else if (receivedDataESP == "M1_OFF") {
        lcd.print("Siri M1_OFF");

      } else if (receivedDataESP == "M2_ON") {
        lcd.print("Siri M2_ON");

      } else if (receivedDataESP == "M2_OFF") {
        lcd.print("Siri M2_OFF");

      } else if (receivedDataESP == "M3_ON") {
        lcd.print("Siri M3_ON");

      } else if (receivedDataESP == "M3_OFF") {
        lcd.print("Siri M3_OFF");

      } else if (receivedDataESP.startsWith("DHT11 :")) {
        lcd.clear(); // เคลียร์จอทุกครั้งก่อนแสดงข้อความใหม่
        
        lcd.setCursor(0, 0);
        lcd.print("Temp: " + receivedDataESP.substring(22, 27) + " \xDF""C");

        lcd.setCursor(0, 1);
        lcd.print("Humidity: " + receivedDataESP.substring(44, 46 ) + " %"); 
   
      } else if (receivedDataESP.startsWith("DS18B20 :")) {
        lcd.clear(); // เคลียร์จอทุกครั้งก่อนแสดงข้อความใหม่
        
        lcd.setCursor(0, 0);
        lcd.print("Temp: " + receivedDataESP.substring(24, 29) + " \xDF""C"); // แสดงค่า 32.70 °C

        lcd.setCursor(0, 1);
        lcd.print("Humidity: nan %"); // แสดงค่า 53%

      }else {
        lcd.print("Unknown CMD");
      }
      
      receivedDataESP = "";
    } else {
      receivedDataESP += inChar;
    }
  }
  delay(500);
}
