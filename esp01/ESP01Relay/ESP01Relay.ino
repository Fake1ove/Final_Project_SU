// https://www.allnewstep.com/product/1932/relay-esp-01-esp-01s-relay-esp8266-01-esp8266-01s-relay

#include "ESP8266WiFi.h"
#define RELAY 0
#define LED 2
const char* ssid = "********" ; //กำหนด SSID  ""
const char* password = "**********" ; //กำหนด Password   ""
unsigned char status_RELAY = 0;
WiFiServer server(80);

void setup() {
Serial.begin(19200); // 19200
pinMode(RELAY, OUTPUT);
pinMode(LED, OUTPUT);
digitalWrite(LED, HIGH);
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED)
{
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");
server.begin();
Serial.println("Server started");
Serial.println(WiFi.localIP()); // แสดงเลข IP ของ ESP8266
digitalWrite(LED, LOW);
}

void loop() {
WiFiClient client = server.available();
if (!client) {
return;
}

Serial.println("new client");
while (!client.available())
{
delay(1);
}
String req = client.readStringUntil('\r');
Serial.println(req);
client.flush();
if (req.indexOf("/RELAYoff") != -1)
{

status_RELAY = 0;
digitalWrite(RELAY, LOW);
Serial.println("RELAY OFF");
}
else if (req.indexOf("/RELAYon") != -1)

{
status_RELAY = 1;
digitalWrite(RELAY, HIGH);
Serial.println("RELAY ON");
}
//Code HTML
String web = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
web += "\r\n"; web += "\r\n";
web += " RELAY Status \r\n";

web += "\r\n";
if (status_RELAY == 1)
web += "RELAY On\r\n";
else
web += "RELAY Off\r\n";
web += "\r\n";
web += "\r\n"; web += "\r\n";
client.print(web); //สร้างโคด HTML
}
