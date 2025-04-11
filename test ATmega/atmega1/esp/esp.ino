void setup() {
  Serial.begin(115200); // เริ่มต้นการสื่อสารผ่าน Serial ที่ baud rate 115200
}

void loop() {
  Serial.println("M1_ON");
  delay(1000);
  Serial.println("M1_OFF");
  delay(1000);
}
