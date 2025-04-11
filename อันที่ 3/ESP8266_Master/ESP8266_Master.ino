#include <ESP8266WiFi.h>
#include <espnow.h>

// REPLACE WITH RECEIVER MAC ADDRESSES
uint8_t receiverMac1[] = {0x5C, 0xCF, 0x7F, 0x06, 0xF4, 0xC2}; // ตัวรับที่ 1
uint8_t receiverMac2[] = {0x5C, 0xCF, 0x7F, 0x85, 0x50, 0x17}; // ตัวรับที่ 2

// Structure example to send data
typedef struct test_struct {
    int x;
    int y;
} test_struct;

test_struct test;

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

// ฟังก์ชันที่ใช้ในการส่งข้อมูลผ่าน Serial
void sendDataToESP(int x, int y) {
  // ส่งข้อมูลไปยัง ESP8266
  Serial.print("ส่งข้อมูลไป ESP8266: x=");
  Serial.print(x);
  Serial.print(", y=");
  Serial.println(y);
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
    esp_now_add_peer(receiverMac1, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
    esp_now_add_peer(receiverMac2, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}
void loop() {
  // ตัวอย่างการเรียกฟังก์ชัน sendDataToESP
  // คุณสามารถส่งค่าของ x และ y จากที่ไหนก็ได้ตามต้องการ
  int x = 1;  // ตัวอย่างค่า x
  int y = 1;  // ตัวอย่างค่า y
  sendDataToESP(x, y);  // เรียกใช้งานฟังก์ชัน

  // บันทึกค่าที่ส่งไปในตัวแปร test
  test.x = x;
  test.y = y;

  // ส่งข้อมูลไปยัง Receiver 1
  esp_now_send(receiverMac1, (uint8_t *)&test, sizeof(test));
  Serial.println("Data sent to Receiver 1");

  // ส่งข้อมูลไปยัง Receiver 2
  esp_now_send(receiverMac2, (uint8_t *)&test, sizeof(test));
  Serial.println("Data sent to Receiver 2");

  delay(1000);  // หน่วงเวลา 1 วินาทีเพื่อไม่ให้ส่งข้อมูลถี่เกินไป
}