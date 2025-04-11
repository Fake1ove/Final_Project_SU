#include <ESP8266WiFi.h>
#include <espnow.h>

// ตั้งค่าให้ใช้ D1 เป็นขาที่เชื่อมต่อกับ LED
#define LED_PIN D1

// Buffer สำหรับเก็บข้อมูลที่รับเข้ามา
char receivedData[10];

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  // คัดลอกข้อมูลที่รับมาเก็บใน buffer
  memcpy(receivedData, incomingData, len);
  receivedData[len] = '\0'; // เติม null-terminator เพื่อให้เป็น string
  Serial.print("Data received: ");
  Serial.println(receivedData);

  // ตรวจสอบข้อความที่ได้รับ
  if (strcmp(receivedData, "G1_ON") == 0) {
    digitalWrite(LED_PIN, HIGH);  // เปิดไฟ
    Serial.println("LED ON");
  } else if (strcmp(receivedData, "G1_OFF") == 0) {
    digitalWrite(LED_PIN, LOW);   // ปิดไฟ
    Serial.println("LED OFF");
  } else {
    Serial.println("Invalid command received");
  }
  Serial.println();
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // ตั้งค่า D1 เป็นขา OUTPUT สำหรับ LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // เริ่มต้นด้วยการปิดไฟ

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register callback function for receiving data
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // ไม่มีการใช้งานอื่นๆในลูป
}
