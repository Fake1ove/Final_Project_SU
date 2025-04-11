#include <ESP8266WiFi.h>
#include <espnow.h>

#define RELAY_PIN 0  // ขา Relay (ESP-01 ใช้ GPIO0 หรือเปลี่ยนเป็น GPIO2)
#define LED_PIN 2   // ขา LED (ESP-01 ใช้ GPIO2)

unsigned char status_RELAY = 0;
char receivedCommand[10];

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH); // ปิด Relay (ESP-01 Relay ใช้ Active LOW)
  digitalWrite(LED_PIN, LOW);   // ปิด LED
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
}

// ฟังก์ชัน Callback เมื่อได้รับข้อมูล
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  char receivedData[10];
  memcpy(receivedData, incomingData, len);
  receivedData[len] = '\0';

  Serial.print("Command received: ");
  Serial.println(receivedData);

  if (strcmp(receivedData, "G2_ON") == 0) {  
    digitalWrite(RELAY_PIN, LOW);  // เปิด Relay
    digitalWrite(LED_PIN, HIGH);    // เปิด LED
    status_RELAY = 1;
    Serial.println("RELAY ON");
  } 
  else if (strcmp(receivedData, "G2_OFF") == 0) {  
    digitalWrite(RELAY_PIN, HIGH); // ปิด Relay
    digitalWrite(LED_PIN, LOW);   // ปิด LED
    status_RELAY = 0;
    Serial.println("RELAY OFF");
  } 
  else {
    Serial.println("Unknown command");
  }
}
