#include <ESP8266WiFi.h>
#include <espnow.h>

uint8_t receiverMac1[] = {0x5c, 0xcf, 0x7f, 0x06, 0xf4, 0xc2}; // 8266 สี B
uint8_t receiverMac2[] = {0x84, 0x0d, 0x8e, 0x97, 0xa0, 0xe3}; // esp01_DHT11
uint8_t receiverMac3[] = {0x50, 0x02, 0x91, 0xce, 0x3c, 0x1a}; // esp01_DS18B20
uint8_t receiverMac4[] = {0x2c, 0xf4, 0x32, 0x76, 0x1f, 0x78}; // esp01_relay01_Light
uint8_t receiverMac5[] = {0x2c, 0xf4, 0x32, 0x76, 0x16, 0x1c}; // esp01_relay02_Fan

// โครงสร้างข้อมูลสำหรับเซ็นเซอร์ DHT11
typedef struct struct_message {
  float temperature;  // อุณหภูมิจาก DHT11
  float humidity;     // ความชื้นจาก DHT11
} struct_message;

struct_message sensorData;  // โครงสร้างข้อมูลเซ็นเซอร์

// โครงสร้างข้อมูลสำหรับเซ็นเซอร์ DS18B20
typedef struct struct_message2 {
  float temperatureDS18B20; // อุณหภูมิจาก DS18B20
} struct_message2;
struct_message sensorData2;


String receivedDataATmega = ""; 

///////////////////////////// END include /////////////////////////////////

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  Serial.print("Master MAC Address: ");
  Serial.println(WiFi.macAddress());


  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_add_peer(receiverMac1, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  esp_now_add_peer(receiverMac2, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  esp_now_add_peer(receiverMac3, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  esp_now_add_peer(receiverMac4, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  esp_now_add_peer(receiverMac5, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

///////////////////////////// END setup /////////////////////////////////

char sendData[10];
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
}

///////////////////////////// END sendData /////////////////////////////////

void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  
  if (len == sizeof(struct_message)) { 
    // รับค่าจาก DHT11
    memcpy(&sensorData, incomingData, sizeof(sensorData)); 
    Serial.print("DHT11 : Temperature : ");
    Serial.print(sensorData.temperature, 2); // แสดงอุณหภูมิเป็นทศนิยม 2 ตำแหน่ง
    Serial.print(" °C ");
    Serial.print(": Humidity :");
    Serial.print((int)sensorData.humidity); // แสดงค่าความชื้นเป็นจำนวนเต็ม
    Serial.println(" %");

  } else if (len == sizeof(struct_message2)) { 
    // รับค่าจาก DS18B20
    struct_message2 sensorData2;
    memcpy(&sensorData2, incomingData, sizeof(sensorData2)); 
    Serial.print("DS18B20 : Temperature : ");
    Serial.print(sensorData2.temperatureDS18B20);
    Serial.println(" °C");
  
  } else { 
    // รับคำสั่งข้อความแทนข้อมูลเซ็นเซอร์
    char receivedCommand[10];
    memcpy(receivedCommand, incomingData, len);
    receivedCommand[len] = '\0';  

    if (strcmp(receivedCommand, "M1_ON") == 0) {
      Serial.println("M1_ON");
    } else if (strcmp(receivedCommand, "M1_OFF") == 0) {
      Serial.println("M1_OFF");
    } else if (strcmp(receivedCommand, "M2_ON") == 0) {
      Serial.println("M2_ON");
    } else if (strcmp(receivedCommand, "M2_OFF") == 0) {
      Serial.println("M2_OFF");
    } else if (strcmp(receivedCommand, "M3_ON") == 0) {
      Serial.println("M3_ON");
    } else if (strcmp(receivedCommand, "M3_OFF") == 0) {
      Serial.println("M3_OFF");
    } else {
      // Serial.println(receivedCommand);
    }
  }
}

///////////////////////////// END OnDataRecv /////////////////////////////////

void loop() {
  while (Serial.available()) {
    char inChar = (char)Serial.read(); 
    if (inChar == '\n') { // เมื่อข้อความครบแล้ว (จบที่ '\n')
      receivedDataATmega.trim(); 

      if (receivedDataATmega == "G1_ON") {
        strcpy(sendData, "G1_ON");

      } else if (receivedDataATmega == "G1_OFF") {
        strcpy(sendData, "G1_OFF");

      } else if (receivedDataATmega == "G2_ON") {
        strcpy(sendData, "G2_ON");

      } else if (receivedDataATmega == "G2_OFF") {
        strcpy(sendData, "G2_OFF");

      } else if (receivedDataATmega == "G3_ON") {
        strcpy(sendData, "G3_ON");

      } else if (receivedDataATmega == "G3_OFF") {
        strcpy(sendData, "G3_OFF");

      } else if (receivedDataATmega == "S1_ON") {
        strcpy(sendData, "S1_ON");

      } else if (receivedDataATmega == "S2_ON") {
        strcpy(sendData, "S2_ON");

      } else {
        receivedDataATmega = ""; // ถ้าไม่ใช่ข้อความที่ต้องการ ให้เคลียร์ buffer
        return; 
      }

      // ส่งข้อมูล ESP-NOW หลังจากข้อความถูกต้อง
      esp_now_send(receiverMac1, (uint8_t *)sendData, strlen(sendData));
      esp_now_send(receiverMac2, (uint8_t *)sendData, strlen(sendData));
      esp_now_send(receiverMac3, (uint8_t *)sendData, strlen(sendData));
      esp_now_send(receiverMac4, (uint8_t *)sendData, strlen(sendData));
      esp_now_send(receiverMac5, (uint8_t *)sendData, strlen(sendData));
      receivedDataATmega = ""; // รีเซ็ตข้อความหลังจากส่งแล้ว
    } else {
      receivedDataATmega += inChar; // บันทึกข้อความทีละตัว
    }
  }
  delay(1000);
}

///////////////////////////// END loop /////////////////////////////////
