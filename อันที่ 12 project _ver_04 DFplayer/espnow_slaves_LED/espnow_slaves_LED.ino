#include <ESP8266WiFi.h>
#include <espnow.h>

// ตั้งค่าให้ใช้ D1 เป็นขาที่เชื่อมต่อกับ LED
#define LED_PIN D1

// Buffer สำหรับเก็บข้อมูลที่รับเข้ามา
char receivedData[10];

// ค่า MAC Address ของ ESP8266 ตัวปลายทาง
uint8_t masterMAC[] = {0x2c, 0x3a, 0xe8, 0x25, 0xdb, 0x4c}; // เปลี่ยนตาม MAC ของปลายทาง

// Callback สำหรับตรวจสอบสถานะการส่งข้อมูล
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", 
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

  // Serial.print("Packet sent to: ");
  // Serial.print(macStr);
  // Serial.print(" send status: ");
  // Serial.println(sendStatus == 0 ? "Success" : "Failed");
}

// Function สำหรับส่งข้อมูล
void sendData(const char *message) {
executed  if (esp_now_send(masterMAC, (uint8_t *)message, strlen(message)) == 0) {
    Serial.print("send message: ");
    Serial.println(message);
  } else {
    Serial.println("Error initiating send");
  }
}

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  // คัดลอกข้อมูลที่รับมาเก็บใน buffer
  memcpy(receivedData, incomingData, len);
  receivedData[len] = '\0'; // เติม null-terminator เพื่อให้เป็น string
  Serial.println(receivedData);

  // ตรวจสอบข้อความที่ได้รับ
  if (strcmp(receivedData, "S2_ON") == 0) {
    digitalWrite(LED_PIN, HIGH);  // เปิดไฟ
    sendData("M1_ON"); // ส่งข้อมูลไปยัง ESP8266 ตัวอื่น
  } else if (strcmp(receivedData, "S3_ON") == 0) {
    digitalWrite(LED_PIN, LOW);   // ปิดไฟ
    sendData("M1_OFF"); // ส่งข้อมูลไปยัง ESP8266 ตัวอื่น
  } 
  
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

  // Register callback function for sending status
  esp_now_register_send_cb(OnDataSent);

  // Add peer (ESP8266 ตัวปลายทาง)
  esp_now_add_peer(masterMAC, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop() {
  // ไม่มีการใช้งานอื่นๆในลูป
}
