#include <ESP8266WiFi.h>
#include <espnow.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

uint8_t receiverMAC[] = { 0x2C, 0x3A, 0xE8, 0x25, 0xDB, 0x4C };  // Replace with the receiver's MAC address

typedef struct {
  float temperature;
  float humidity;
} SensorData;

SensorData sensorData;
bool sendDataFlag = false; // Flag to check when to send data

void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  char receivedData[10];
  memcpy(receivedData, incomingData, len);
  receivedData[len] = '\0';  // Null-terminate the string

  // Check if the received command is "S2_ON"
  if (strcmp(receivedData, "S2_ON") == 0) {
    sendDataFlag = true; // Set the flag to send data when S2_ON is received
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW Initialization Failed");
    ESP.restart();
  }

  // Set up ESP-NOW as the controller
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  // Add peer (receiver device) to communicate with
  esp_now_add_peer(receiverMAC, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  // Register callback function for receiving data
  esp_now_register_recv_cb(OnDataRecv);

  // Register callback for sending status (optional)
  esp_now_register_send_cb([](uint8_t* mac, uint8_t sendStatus) {
    Serial.print("Send status: ");
    Serial.println(sendStatus == 0 ? "Success" : "Failed");
  });

  Serial.println("ESP-NOW Initialized");
}

void loop() {
  // อ่านค่าจากเซ็นเซอร์ DHT11
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  // ตรวจสอบว่าค่าที่อ่านได้ไม่เป็น NaN
  if (!isnan(temp) && !isnan(hum)) {
    if (sendDataFlag) {
      // แปลงค่า Temperature ให้เป็นทศนิยม 2 ตำแหน่ง
      char tempStr[10];
      dtostrf(temp, 4, 2, tempStr); // แปลงเป็นทศนิยม 2 ตำแหน่ง

      // กำหนดค่าในโครงสร้างข้อมูลก่อนส่ง
      sensorData.temperature = atof(tempStr); // แปลงกลับเป็น float
      sensorData.humidity = (int)hum; // แปลง Humidity เป็นจำนวนเต็ม

      // ส่งข้อมูลผ่าน ESP-NOW
      esp_now_send(receiverMAC, (uint8_t*)&sensorData, sizeof(sensorData));

      // แสดงผลใน Serial Monitor
      Serial.println("Data Sent: ");
      Serial.print("Temperature: ");
      Serial.print(sensorData.temperature, 2); // แสดงทศนิยม 2 ตำแหน่ง
      Serial.print(", Humidity: ");
      Serial.println(sensorData.humidity); // แสดงเป็นจำนวนเต็ม

      sendDataFlag = false; // รีเซ็ต flag หลังจากส่งข้อมูลแล้ว
    }
  } else {
    Serial.println("Failed to read from DHT sensor!");
  }

  delay(2000); // หน่วงเวลา 2 วินาที
}
