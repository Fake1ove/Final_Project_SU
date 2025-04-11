#include <ESP8266WiFi.h>
#include <espnow.h>

uint8_t slaveMAC[] = {0x84, 0x0D, 0x8E, 0x97, 0xA0, 0xE3}; // MAC Address ของ Slave

typedef struct struct_message {
  float temperature;
  float humidity;
} struct_message;

struct_message receivedData;

// ฟังก์ชันส่งข้อมูลไปยัง Slave
void sendData(const char *message) {
  Serial.print("Sending command: ");
  Serial.println(message);

  if (esp_now_send(slaveMAC, (uint8_t *)message, strlen(message)) == 0) {
    Serial.println("Command sent successfully");
  } else {
    Serial.println("Error sending command");
  }
}

// ฟังก์ชัน callback เมื่อได้รับข้อมูลจาก Slave
void dataReceived(uint8_t *senderMac, uint8_t *incomingData, uint8_t len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));
  Serial.print("Temperature: ");
  Serial.print(receivedData.temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(receivedData.humidity);
  Serial.println(" %");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // ตรวจสอบการเริ่มต้น ESP-NOW
  if (esp_now_init() == 0) {
    Serial.println("ESP-NOW Initialized.");
    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
    esp_now_register_recv_cb(dataReceived);
  } else {
    Serial.println("ESP-NOW Initialization Failed.");
    ESP.restart();
  }
}

void loop() {
  // ตรวจสอบว่ามีข้อมูลใน Serial monitor หรือไม่
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n'); // อ่านคำสั่งจาก Serial monitor

    // ตรวจสอบคำสั่งที่ได้รับจาก Serial
    if (command == "S2_ON") {
      sendData("S2_ON"); // ส่งคำสั่ง "S2_ON" ไปยัง slave
    }
  }
}
