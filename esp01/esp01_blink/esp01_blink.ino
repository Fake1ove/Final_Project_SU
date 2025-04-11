void setup() {
  pinMode(2, OUTPUT); // GPIO2 คือตัวควบคุม LED บน ESP-01
}

void loop() {
  digitalWrite(2, HIGH);
  delay(1000);
  digitalWrite(2, LOW);
  delay(1000);
}
