#include <ESP8266WiFi.h>
#include <espnow.h>

#define RELAY_PIN 0  // ขา Relay (ESP-01 ใช้ GPIO0)
#define LED_PIN 2    // ขา LED (ESP-01 ใช้ GPIO2)

unsigned char status_RELAY = 0;
char receivedData[10]; 
uint8_t masterMAC[] = {0x2C, 0x3A, 0xE8, 0x25, 0xDB, 0x4C};  // MAC Address ของ Master

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH);  // ปิด Relay (Active LOW)
  digitalWrite(LED_PIN, LOW);    // ปิด LED

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(masterMAC, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

// ฟังก์ชันส่งข้อมูลกลับไปยัง Master
void sendData(const char *message) {
  if (esp_now_send(masterMAC, (uint8_t *)message, strlen(message)) == 0) {
    Serial.print("Sent message: ");
    Serial.println(message);
  } else {
    Serial.println("Error sending data");
  }
}

// ฟังก์ชัน Callback เมื่อส่งข้อมูลสำเร็จ/ล้มเหลว
void OnDataSent(uint8_t *mac, uint8_t status) {
  Serial.print("Send status: ");
  Serial.println(status == 0 ? "Success" : "Failed");
}

// ฟังก์ชัน Callback เมื่อได้รับข้อมูล
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  if (len >= sizeof(receivedData)) {  // ป้องกัน Buffer Overflow
    Serial.println("Received data");
    return;
  }

  memcpy(receivedData, incomingData, len);
  receivedData[len] = '\0'; // เพิ่ม Null Terminator

  Serial.print("Command received: ");
  Serial.println(receivedData);

  if (strcmp(receivedData, "G1_ON") == 0) {  
    digitalWrite(RELAY_PIN, LOW);  // เปิด Relay
    digitalWrite(LED_PIN, HIGH);    // เปิด LED
    status_RELAY = 1;
    Serial.println("RELAY ON");
    sendData("M1_ON");
  } 
  else if (strcmp(receivedData, "G1_OFF") == 0) {  
    digitalWrite(RELAY_PIN, HIGH); // ปิด Relay
    digitalWrite(LED_PIN, LOW);   // ปิด LED
    status_RELAY = 0;
    Serial.println("RELAY OFF");
    sendData("M1_OFF");
  } 
  else {
    Serial.println("Unknown command");
  }
}

void loop() {
}
