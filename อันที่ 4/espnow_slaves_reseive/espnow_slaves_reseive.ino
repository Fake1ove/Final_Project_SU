#include <ESP8266WiFi.h>
#include <espnow.h>

// ตั้งค่าให้ใช้ D1 เป็นขาที่เชื่อมต่อกับ LED
#define LED_PIN D1

// Structure example to receive data
typedef struct test_struct {
    int x;
    int y;
} test_struct;

test_struct myData;

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("x: ");
  Serial.println(myData.x);
  Serial.print("y: ");
  Serial.println(myData.y);
  
  // เปิดหรือปิดไฟ LED ตามข้อมูลที่ได้รับ
  if (myData.x == 1 && myData.y == 1) {
    digitalWrite(LED_PIN, HIGH);  // เปิดไฟ
    Serial.println("LED ON");
  } else if (myData.x == 0 && myData.y == 0) {
    digitalWrite(LED_PIN, LOW);   // ปิดไฟ
    Serial.println("LED OFF");
  } else {
    Serial.println("Invalid data received");
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
