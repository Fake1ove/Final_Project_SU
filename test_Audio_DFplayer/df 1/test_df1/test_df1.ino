#include "Arduino.h"
#include "EasyVR.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial easyvrPort(12, 13); // RX, TX สำหรับ EasyVR
EasyVR easyvr(easyvrPort);

SoftwareSerial dfPlayerPort(8, 9); // RX, TX สำหรับ DFPlayer Mini
DFRobotDFPlayerMini myDFPlayer;

#define pcSerial Serial

enum Groups {
  GROUP_0 = 0,
  GROUP_1 = 1,
};

enum Group0 {
  G0_LED = 0,
};

enum Group1 {
  G1_ON = 0,
  G1_OFF = 1,
};

int8_t group, idx;
String receivedDataESP = "";

void setup() {
  Serial.begin(115200);
  easyvrPort.begin(9600);
  dfPlayerPort.begin(9600);

  pinMode(11, OUTPUT);
  pinMode(7, OUTPUT);

  digitalWrite(11, LOW);
  digitalWrite(7, LOW);

  if (!myDFPlayer.begin(dfPlayerPort)) {
    Serial.println("DFPlayer Mini ไม่สามารถเริ่มต้นได้!");
    while (true);
  }
  Serial.println("DFPlayer Mini พร้อมใช้งาน");
  myDFPlayer.volume(30); // ระดับเสียง 0-30

  while (!easyvr.detect()) {
    Serial.println("EasyVR not detected!");
    delay(1000);
  }

  easyvr.setPinOutput(EasyVR::IO1, LOW);
  Serial.println("EasyVR detected!");
  easyvr.setTimeout(5);
  easyvr.setLanguage(0);

  group = EasyVR::TRIGGER;
  easyvr.playSound(10, EasyVR::VOL_FULL);
}

void loop() {
  Serial.println("Now in loop...");
  easyvr.setPinOutput(EasyVR::IO1, HIGH);

  Serial.print("Say a command in Group ");
  Serial.println(group);
  easyvr.recognizeCommand(group);

  while (!easyvr.hasFinished()); // รอการประมวลผลคำสั่งเสียง

  easyvr.setPinOutput(EasyVR::IO1, LOW);
  easyvr.setMicDistance(3);
  easyvr.setLevel(1);
  easyvr.setKnob(0);

  idx = easyvr.getWord();
  if (idx >= 0) return;

  idx = easyvr.getCommand();
  if (idx >= 0) {
    char name[32];  // ✅ ใช้ตัวแปรเดียว
    uint8_t train = 0;  // ✅ ใช้ตัวแปรที่สามารถอ้างอิงได้

    Serial.print("Command: ");
    Serial.print(idx);

    if (easyvr.dumpCommand(group, idx, name, train))  // ✅ ใช้ตัวแปรแทนค่า 0
    {
        Serial.print(" = ");
        Serial.println(name);
    }

    easyvr.playSound(0, EasyVR::VOL_FULL);
    action();
  }

  if (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      receivedDataESP.trim();
      if (receivedDataESP == "M1_ON") {
        myDFPlayer.play(1);
      } else if (receivedDataESP == "M1_OFF") {
        myDFPlayer.play(2);
      }
      receivedDataESP = "";
    } else {
      receivedDataESP += inChar;
    }
  }

  delay(500);
}

void action() {
  switch (group) {
    case GROUP_0:
      if (idx == G0_LED) {
        group = GROUP_1;
      }
      break;

    case GROUP_1:
      if (idx == G1_ON) {
        digitalWrite(11, HIGH);
        Serial.println("G1_ON");
      } else if (idx == G1_OFF) {
        digitalWrite(11, LOW);
        Serial.println("G1_OFF");
      }
      break;
  }
}
