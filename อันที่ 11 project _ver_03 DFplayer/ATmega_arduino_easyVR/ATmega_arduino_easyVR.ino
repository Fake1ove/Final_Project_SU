  #include "Arduino.h"
#if !defined(SERIAL_PORT_MONITOR)
#endif

#if defined(SERIAL_PORT_USBVIRTUAL)
  // Shield Jumper on HW (for Leonardo and Due)
  #define port SERIAL_PORT_HARDWARE
  #define pcSerial SERIAL_PORT_USBVIRTUAL
#else

  #include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

  // Shield Jumper on SW (using pins 12/13 or 8/9 as RX/TX)
  #include "SoftwareSerial.h"

  #include "EasyVR.h"
SoftwareSerial easyvrPort(12, 13); // RX, TX สำหรับ EasyVR
EasyVR easyvr(easyvrPort);

//   #include "DFRobotDFPlayerMini.h"
// SoftwareSerial dfPlayerPort(8, 9); // RX, TX สำหรับ DFPlayer Mini
// DFRobotDFPlayerMini myDFPlayer;

  #define pcSerial SERIAL_PORT_MONITOR
#endif
#pragma once   

enum Groups {
  GROUP_0 = 0,
  GROUP_1 = 1,
};

enum Group0 {
  SIRI = 0,
};

enum Group1 {
  G1_ON  = 0,
  G1_OFF = 1,
  G2_ON  = 2,
  G2_OFF = 3,
  G3_ON  = 4,
  G3_OFF = 5,
  S1_ON  = 6,
  S2_ON  = 7,
  S3_ON  = 8
};

int8_t group , idx;

String receivedDataESP = "";

///////////////////////////// END include /////////////////////////////////

void setup() {
  Serial.begin(115200);
  easyvrPort.begin(115200);
  // dfPlayerPort.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);

  //   if (!myDFPlayer.begin(dfPlayerPort)) {
  //   lcd.clear();
  //   lcd.print("NO DFPlayer");
  //   Serial.println("EasyVR not detected!");
  //   while (true);
  // }

  lcd.clear();
  lcd.print("Siri Test");
  // myDFPlayer.volume(30); // ระดับเสียง 0-30// ตั้งระดับเสียง (0-30)

int mode = easyvr.bridgeRequested(pcSerial);
  switch (mode)
  {
    case EasyVR::BRIDGE_NONE:
       easyvrPort.begin(9600);
      pcSerial.println(F("---"));
      pcSerial.println(F("Bridge not started!"));
      Serial.println("*** case EasyVR::BRIDGE_NONE: ***");
      break;

    case EasyVR::BRIDGE_NORMAL:
      easyvrPort.begin(9600);
      easyvr.bridgeLoop(pcSerial);
      pcSerial.println(F("---"));
      pcSerial.println(F("Bridge connection aborted!"));
      Serial.println("*** case EasyVR::BRIDGE_NORMAL: ***");
      break;

    case EasyVR::BRIDGE_BOOT:
      easyvrPort.begin(115200);
      easyvr.bridgeLoop(pcSerial);
      pcSerial.println(F("---"));
      pcSerial.println(F("Bridge connection aborted!"));
      Serial.println("*** case EasyVR::BRIDGE_BOOT: ***");
      break;
  }

  easyvr.setPinOutput(EasyVR::IO1, LOW);
  Serial.println("EasyVR detected!");
  easyvr.setTimeout(5);
  easyvr.setLanguage(0);

  group = EasyVR::TRIGGER;


  while (!easyvr.detect()) {
    lcd.clear();
    lcd.print("NO EasyVR");
    Serial.println("EasyVR not detected!");
    delay(1000);
  }

}
///////////////////////////// END setup /////////////////////////////////

void action() {
  switch (group) {
    case GROUP_0:
      if (idx == SIRI) {
        group = GROUP_1;
        lcd.clear(); // เคลียร์จอทุกครั้งก่อนแสดงข้อความใหม่
        lcd.setCursor(0, 0);
        lcd.print("Siri Hi");
        // myDFPlayer.play(1);
      }
      break;

    case GROUP_1:
      if (idx == G1_ON) {
        Serial.println("G1_ON");
        lcd.setCursor(0, 0);
        lcd.print("Siri Relay 01");
        lcd.setCursor(0, 1);
        lcd.print("ON.");
        // myDFPlayer.play(1);

      } else if (idx == G1_OFF) {
        Serial.println("G1_OFF");
        lcd.setCursor(0, 0);
        lcd.print("Siri Relay 01");
        lcd.setCursor(0, 1);
        lcd.print("OFF.");
        // myDFPlayer.play(1);


      } else if (idx == G2_ON) {
        Serial.println("G2_ON");
        lcd.setCursor(0, 0);
        lcd.print("Siri Relay 02");
        lcd.setCursor(0, 1);
        lcd.print("ON.");

      } else if (idx == G2_OFF) {
        Serial.println("G2_OFF");
        lcd.setCursor(0, 0);
        lcd.print("Siri Relay 02");
        lcd.setCursor(0, 1);
        lcd.print("OFF.");

      } else if (idx == G3_ON) {
        Serial.println("G3_ON");
        lcd.setCursor(0, 0);
        lcd.print("Siri Relay 03");
        lcd.setCursor(0, 1);
        lcd.print("ON.");

      } else if (idx == G3_OFF) {
        Serial.println("G3_OFF");
        lcd.setCursor(0, 0);
        lcd.print("Siri Relay 03");
        lcd.setCursor(0, 1);
        lcd.print("OFF.");
 
      } else if (idx == S1_ON) {
        Serial.println("S1_ON");
        lcd.setCursor(0, 0);
        lcd.print("Siri Sensor");
        lcd.setCursor(0, 1);
        lcd.print("DS18B20");

      } else if (idx == S2_ON) {
        Serial.println("S2_ON");
        lcd.setCursor(0, 0);
        lcd.print("Siri Sensor");
        lcd.setCursor(0, 1);
        lcd.print("DHT11");

      }
      break;
  }
}

///////////////////////////// END action /////////////////////////////////

void loop() {
  Serial.println("Now in loop...");
  
  easyvr.setPinOutput(EasyVR::IO1, HIGH); // LED on (listening)

  // Serial.print("Say a command in Group ");
  // Serial.println(group);
  easyvr.recognizeCommand(group); // Recognition of a built-in word.

  do
  {
    // can do some processing while waiting for a spoken command
  }
  while (easyvr.hasFinished()); // Polls the status of ongoing recognition
    easyvr.setPinOutput(EasyVR::IO1, LOW); // LED off

    easyvr.setMicDistance(3); // Set mic operating distance. 1 nearest - 3 farthest.
    easyvr.setLevel(1); // Set strictness level for recognition of custom commands.
    easyvr.setKnob(0); // Set confidence threshold for recognition of built-in words.

  idx = easyvr.getWord(); // Get recognized word index from built-in sets or custom grammars.
  if (idx >= 0)
  {
    return;
  } // end if.

  idx = easyvr.getCommand(); 

 if (idx >= 0)
  {
    // print debug message
    uint8_t train = 0;
    char name[32];
    Serial.print("Command: ");
    Serial.print(idx);
    if (easyvr.dumpCommand(group, idx, name, train)) // Retrieves the name and training data of a custom command.
    {
      Serial.print(" = ");
      Serial.println(name);
    } // end if.

    else
        Serial.print(" no ");
    // perform some action
    action(); // Turn LED on or off.
    
  }

  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      receivedDataESP.trim();
      lcd.clear(); // เคลียร์จอทุกครั้งก่อนแสดงข้อความใหม่
      lcd.setCursor(0, 0);

      if (receivedDataESP.startsWith("DHT11 :")) {
        lcd.clear(); // เคลียร์จอทุกครั้งก่อนแสดงข้อความใหม่
        lcd.setCursor(0, 0);
        lcd.print("DHT11");

        lcd.setCursor(0, 1);
        lcd.print("T:" + receivedDataESP.substring(22, 27) + "\xDF""C");
        lcd.print(" H:" + receivedDataESP.substring(44, 46 ) + "%"); 
        
        easyvr.setPinOutput(EasyVR::IO1, HIGH); // LED on (listening)
        easyvr.recognizeCommand(group); // Recognition of a built-in word.


      } else if (receivedDataESP.startsWith("DS18B20 :")) {
        lcd.clear(); // เคลียร์จอทุกครั้งก่อนแสดงข้อความใหม่

        lcd.setCursor(0, 0);
        lcd.print("DS18B20");

        lcd.setCursor(0, 1);
        lcd.print("T:" + receivedDataESP.substring(24, 29) + "\xDF""C"); 
        lcd.print(" H:nan%"); 

        easyvr.setPinOutput(EasyVR::IO1, HIGH); // LED on (listening)
        easyvr.recognizeCommand(group); // Recognition of a built-in word.
      }

      receivedDataESP = "";

    } else {
      receivedDataESP += inChar;
    }
  }
  delay(500);
}

///////////////////////////// END loop /////////////////////////////////

