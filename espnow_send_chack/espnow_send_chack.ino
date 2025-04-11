#include <ESP8266WiFi.h>
#include <espnow.h>

// กำหนด MAC Address ของตัวรับ
uint8_t receiverMac[] = {0x2c, 0xf4, 0x32, 0x76, 0x1f, 0x78};

// สร้างโครงสร้างข้อมูลที่ต้องการส่ง
typedef struct test_struct {
    char message[10]; // ใช้เก็บข้อความ
} test_struct;

test_struct test;

// Callback เมื่อส่งข้อมูลเสร็จ
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
    Serial.print("Packet to: ");
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac_addr[0], mac_addr[1], mac_addr[2],
             mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.print(macStr);
    Serial.print(" send status: ");
    Serial.println(sendStatus == 0 ? "Delivery success" : "Delivery fail");
}

void setup() {
    // เริ่มต้น Serial Monitor
    Serial.begin(115200);

    // ตั้งค่าเป็น Wi-Fi Station
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // เริ่มต้น ESP-NOW
    if (esp_now_init() != 0) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    esp_now_register_send_cb(OnDataSent);

    // ลงทะเบียนตัวรับข้อมูล (Peer)
    esp_now_add_peer(receiverMac, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop() {
    if (Serial.available() > 0) {
        // อ่านข้อมูลจาก Serial ทั้งบรรทัด
        String input = Serial.readStringUntil('\n');
        input.trim(); // ลบช่องว่างหรือ '\r' // ล้าง Buffer ป้องกันข้อมูลค้าง

        // กำหนดข้อความที่ต้องการส่ง
        if (input == "G2_ON") {
            strcpy(test.message, "G2_ON");
        } else if (input == "G2_OFF") {
            strcpy(test.message, "G2_OFF");
        } else if (input == "S1_ON") {
            strcpy(test.message, "S1_ON");
        } else if (input == "S2_ON") {
            strcpy(test.message, "S2_ON");
        } else {
            return; // ไม่ทำอะไรถ้าไม่ใช่ค่าที่กำหนด
        }

        // ส่งข้อมูลไปยังตัวรับ
        esp_now_send(receiverMac, (uint8_t *)&test, sizeof(test));

        // หน่วงเวลาเล็กน้อยก่อนรับค่าถัดไป
        delay(1000);
    }
}
