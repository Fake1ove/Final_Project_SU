#include <ESP8266WiFi.h>
#include <espnow.h>

// REPLACE WITH RECEIVER MAC Addresses
uint8_t receiverMac1[] = {0x5c, 0xcf, 0x7f, 0x06, 0xf4, 0xc2}; // 8266 สี B
uint8_t receiverMac2[] = {0x5c, 0xcf, 0x7f, 0x85, 0x50, 0x17}; // 8266 สี Y
uint8_t receiverMac3[] = {0x84, 0x0d, 0x8e, 0x97, 0xa0, 0xe3}; // esp01

// Buffer สำหรับส่งข้อมูล
char sendData[10];

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Packet to: ");
  Serial.print(macStr);
  Serial.print(" send status: ");
  Serial.println(sendStatus == 0 ? "Delivery success" : "Delivery fail");
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  
  // Register callback
  esp_now_register_send_cb(OnDataSent);
  
  // Register peers
  esp_now_add_peer(receiverMac1, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  esp_now_add_peer(receiverMac2, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  esp_now_add_peer(receiverMac3, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop() {
  // Check if data is available in Serial input
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n'); // อ่านข้อมูลจาก Serial Monitor
    
    input.trim(); // ลบช่องว่างหรืออักขระพิเศษที่ไม่จำเป็น
    Serial.print("Received: ");
    Serial.println(input);  // แสดงข้อมูลที่รับจาก Serial Monitor

    if (input == "G1_ON") {
      // เปิดอุปกรณ์
      strcpy(sendData, "G1_ON");
      Serial.println("Turning ON devices");
    } 
    else if (input == "G1_OFF") {
      // ปิดอุปกรณ์
      strcpy(sendData, "G1_OFF");
      Serial.println("Turning OFF devices");
    } 
    else {
      Serial.println("Invalid input. Please send 'G1_ON' or 'G1_OFF'.");
      return;
    }

    // ส่งข้อมูลไปยังตัวรับทั้งสามทันที
    esp_now_send(receiverMac1, (uint8_t *) &sendData, sizeof(sendData));
    Serial.println("Data sent to Receiver 1");

    esp_now_send(receiverMac2, (uint8_t *) &sendData, sizeof(sendData));
    Serial.println("Data sent to Receiver 2");

    esp_now_send(receiverMac3, (uint8_t *) &sendData, sizeof(sendData));
    Serial.println("Data sent to Receiver 3");

    // แสดงค่าที่ส่ง
    Serial.print("Data content: ");
    Serial.println(sendData);
  }
}
