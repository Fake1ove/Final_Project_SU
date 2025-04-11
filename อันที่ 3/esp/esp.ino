void setup() {
  Serial.begin(115200);
}
void loop() {
  Serial.print("1"); // turn the LED on
  delay(1000); // wait for a second
  Serial.print("0"); // turn the LED off
  delay(1000); // wait for a second
}