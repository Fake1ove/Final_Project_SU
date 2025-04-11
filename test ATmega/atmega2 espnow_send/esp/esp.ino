String receivedDataATmega = ""; 

void setup() {
  Serial.begin(115200); // เริ่มต้นการสื่อสารผ่าน Serial ที่ baud rate 115200
}

void loop() {
  while (Serial.available()) {
    char inChar = (char)Serial.read(); // อ่านทีละตัวอักษร
    if (inChar == '\n') { // สิ้นสุดข้อความ
      receivedDataATmega.trim(); // ลบอักขระพิเศษ
      
      if (receivedDataATmega == "G1_ON") {
        Serial.println("M1_ON");
      } else if (receivedDataATmega == "G1_OFF") {
        Serial.println("M1_OFF");
      }
      
      receivedDataATmega = ""; // รีเซ็ตข้อความ
    } else {
      receivedDataATmega += inChar; // บันทึกข้อความทีละตัว
    }
  }
}