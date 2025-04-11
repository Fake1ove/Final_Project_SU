String receivedDataESP = "";  

void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW); // กำหนดค่าเริ่มต้นให้ LOW
}

void loop() {
  Serial.println("G1_ON");
  digitalWrite(13, HIGH);
  delay(2000);

  Serial.println("G1_OFF");
  digitalWrite(13, LOW);
  delay(2000);

  while (Serial.available()) {
    char inChar = (char)Serial.read();
    
    if (inChar == '\n') {
      receivedDataESP.trim(); // ลบช่องว่างและอักขระพิเศษ

      if (receivedDataESP == "M1_ON") {
        digitalWrite(8, HIGH);
        delay(2000); // รอ 2 วินาทีเพื่อให้มั่นใจว่าไม่เปิด-ปิดเร็วเกินไป
      }else if (receivedDataESP == "M1_OFF") {
        digitalWrite(8, LOW);
        delay(2000);
      }
      receivedDataESP = ""; // รีเซ็ตค่าข้อความ
    }else {
      receivedDataESP += inChar; // บันทึกข้อความทีละตัว
    }
  }
  delay(2000); // ป้องกันการอ่านซ้ำเร็วเกินไป
}
