#include <ESP8266WiFi.h>
#include <espnow.h>

// โครงสร้างข้อมูลที่ใช้ส่งและรับ
typedef struct struct_message {
  int x;
  int y;
} struct_message;

struct_message incomingData = {0, 0}; // ข้อมูลที่รับจาก Master

bool dataReceived = false;            // สถานะการรับข้อมูล
unsigned long lastCheckTime = 0;      // เวลาสำหรับตรวจสอบสถานะ

// ฟังก์ชันสำหรับการรับข้อมูล
void OnDataRecv(uint8_t *mac, uint8_t *incomingDataBuffer, uint8_t len) {
  if (len != sizeof(struct_message)) {
    Serial.println("Error: Data length mismatch");
    return;
  }

  memcpy(&incomingData, incomingDataBuffer, sizeof(struct_message));
  Serial.printf("Received data: x = %d, y = %d\n", incomingData.x, incomingData.y);

  // ควบคุม LED ตามค่า x และ y
  if (incomingData.x == 1 && incomingData.y == 1) {
    digitalWrite(D1, HIGH); // เปิดไฟที่ D1
  } else if (incomingData.x == 0 && incomingData.y == 0) {
    digitalWrite(D1, LOW);  // ปิดไฟที่ D1
  }

  dataReceived = true; // อัปเดตสถานะว่ามีข้อมูลใหม่
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP8266 Slave started...");

  // ตั้งค่า WiFi เป็น Station mode
  WiFi.mode(WIFI_STA);

  // เริ่มต้น ESP-NOW
  if (esp_now_init() != ERR_OK) {
    Serial.println("Error: ESP-NOW initialization failed!");
    return;
  }

  // ลงทะเบียน callback สำหรับการรับข้อมูล
  esp_now_register_recv_cb(OnDataRecv);

  // ตั้งค่า pin D1 เป็น output
  pinMode(D1, OUTPUT);
  digitalWrite(D1, LOW); // ปิด LED เริ่มต้น
}

void loop() {
  unsigned long currentTime = millis();

  // ตรวจสอบทุก 2 วินาที
  if (currentTime - lastCheckTime >= 2000) {
    lastCheckTime = currentTime;
    
    if (dataReceived) {
      Serial.println("Status: Data received successfully");
    } else {
      Serial.println("Status: No data received");
    }

    // แสดงข้อมูลหน่วยความจำที่เหลือ
    Serial.printf("Free heap memory: %d bytes\n", ESP.getFreeHeap());
    dataReceived = false; // รีเซ็ตสถานะการรับข้อมูล
  }
}
