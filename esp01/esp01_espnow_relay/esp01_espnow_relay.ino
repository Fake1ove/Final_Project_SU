#include <ESP8266WiFi.h>
#include <espnow.h>

#define RELAY_PIN 0   // กำหนดขา GPIO สำหรับ Relay (อาจจะต้องปรับตามขาที่ใช้จริง)
#define LED_PIN 2     // กำหนดขา GPIO สำหรับ LED

unsigned char status_RELAY = 0;  // สถานะของ Relay

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);  // ตั้งขา Relay เป็น OUTPUT
  pinMode(LED_PIN, OUTPUT);    // ตั้งขา LED เป็น OUTPUT

  digitalWrite(LED_PIN, HIGH); // LED OFF (เพราะ HIGH สำหรับ LED)
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  // เริ่มต้น ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // ตั้งสถานะให้เป็น Slave
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  
  // ลงทะเบียน Callback ฟังก์ชันที่ใช้เมื่อรับข้อมูล
  esp_now_register_recv_cb(onDataReceived);
}

void loop() {
  // ไม่มีอะไรใน loop() เพราะตัว ESP8266 รอรับข้อมูลจาก ESP-NOW
}

// ฟังก์ชัน Callback ที่ทำงานเมื่อได้รับข้อมูล
void onDataReceived(uint8_t *senderMac, uint8_t *data, uint8_t len) {
  if (*data == 1) {  // ถ้าข้อมูลที่ได้รับเป็น 1
    digitalWrite(RELAY_PIN, HIGH);  // เปิด Relay
    digitalWrite(LED_PIN, LOW);     // เปิด LED
    status_RELAY = 1;
    Serial.println("RELAY ON");
  } 
  else if (*data == 0) {  // ถ้าข้อมูลที่ได้รับเป็น 0
    digitalWrite(RELAY_PIN, LOW);   // ปิด Relay
    digitalWrite(LED_PIN, HIGH);    // ปิด LED
    status_RELAY = 0;
    Serial.println("RELAY OFF");
  }
}
