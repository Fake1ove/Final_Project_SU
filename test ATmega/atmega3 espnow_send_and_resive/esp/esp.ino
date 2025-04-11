String receivedDataATmega = ""; 

#include <ESP8266WiFi.h>
#include <espnow.h>

uint8_t receiverMac1[] = {0x5c, 0xcf, 0x7f, 0x06, 0xf4, 0xc2}; // 8266 สี B

// Callback when data is sent
char sendData[10];
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
}

// Callback function that will be executed when data is received
char receivedData[10];
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memcpy(receivedData, incomingData, len);
  receivedData[len] = '\0'; // เติม null-terminator เพื่อให้เป็น string
  if (strcmp(receivedData, "M1_ON") == 0) {
    Serial.println("M1_ON"); //ส่งข้อมูลไปยัง atmega
  } else if (strcmp(receivedData, "M1_OFF") == 0) {
    Serial.println("M1_OFF"); //ส่งข้อมูลไปยัง atmega
  } 
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_add_peer(receiverMac1, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop() {
  while (Serial.available()) {
    char inChar = (char)Serial.read(); 
    if (inChar == '\n') { // เมื่อข้อความครบแล้ว (จบที่ '\n')
      receivedDataATmega.trim(); 

      if (receivedDataATmega == "G1_ON") {
        strcpy(sendData, "G1_ON");
      } else if (receivedDataATmega == "G1_OFF") {
        strcpy(sendData, "G1_OFF");
      } else {
        receivedDataATmega = ""; // ถ้าไม่ใช่ข้อความที่ต้องการ ให้เคลียร์ buffer
        return; 
      }

      // ส่งข้อมูล ESP-NOW หลังจากข้อความถูกต้อง
      esp_now_send(receiverMac1, (uint8_t *)sendData, strlen(sendData));
      receivedDataATmega = ""; // รีเซ็ตข้อความหลังจากส่งแล้ว
    } else {
      receivedDataATmega += inChar; // บันทึกข้อความทีละตัว
    }
  }
  delay(500);
}
