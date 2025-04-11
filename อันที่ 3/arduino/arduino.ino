void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(115200);
}
void loop() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '1') {
      digitalWrite(13, HIGH);
    }
    else if (inChar == '0') {
      digitalWrite(13, LOW);
    }
  }
}