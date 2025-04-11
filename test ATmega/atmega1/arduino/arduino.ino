String receivedData = ""; 

void setup() {
  Serial.begin(115200); // 
  pinMode(13, OUTPUT); // LED บนบอร์ด
}

void loop() {
  while (Serial.available()) {
    char inChar = (char)Serial.read(); // อ่านทีละตัวอักษร
    if (inChar == '\n') { // สิ้นสุดข้อความ
      if (receivedData.indexOf("M1_ON") >= 0) {
       digitalWrite(13, HIGH); 
      } else if (receivedData.indexOf("M1_OFF") >= 0) {
       digitalWrite(13, LOW);
      }
      receivedData = ""; 
    } else {
      receivedData += inChar; // บันทึกข้อความทีละตัว
    }
  }
}
