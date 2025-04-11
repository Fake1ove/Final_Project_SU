#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
 
SoftwareSerial mySoftwareSerial(2, 3); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
 
void setup()
{
mySoftwareSerial.begin(9600);
Serial.begin(9600);
 
Serial.println();
Serial.println(F("DFRobot DFPlayer Mini Demo"));
Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
 
if (!myDFPlayer.begin(mySoftwareSerial)) { //Use softwareSerial to communicate with mp3.
Serial.println(F("Unable to begin:"));
Serial.println(F("1.Please recheck the connection!"));
Serial.println(F("2.Please insert the SD card!"));
while (true) {
delay(1000); // Code to compatible with ESP8266 watch dog.
}
}
Serial.println(F("DFPlayer Mini online."));
 
myDFPlayer.volume(30); //ตั้งระดับความดังของเสียง 0-30
}
 
void loop()
{
while (Serial.available()) {
char inChar = (char)Serial.read();
if (inChar == '1') {
myDFPlayer.play(1);
}
if (inChar == '2') {
myDFPlayer.play(2);
}
if (inChar == '3') {
myDFPlayer.play(3);
}
else if (inChar == '0') {
myDFPlayer.stop();
}
}
}
