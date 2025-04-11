#include <ESP8266WiFi.h>
#include <espnow.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2

uint8_t receiverMAC[] = { 0x2C, 0x3A, 0xE8, 0x25, 0xDB, 0x4C };  // Replace with the receiver's MAC address

typedef struct {
  float temperatureDS18B20 ;
} SensorData;

SensorData sensorData;
bool sendDataFlag = false; // Flag to check when to send data

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  char receivedData[10];
  memcpy(receivedData, incomingData, len);
  receivedData[len] = '\0';  // Null-terminate the string

  // Check if the received command is "S2_ON"
  if (strcmp(receivedData, "S1_ON") == 0) {
    sendDataFlag = true; // Set the flag to send data when S2_ON is received
  }
}

void setup() {
  Serial.begin(115200);
  sensors.begin(); // เซนเซอร์ DS18B20 เริ่มต้นทำงาน
  
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
  // Read values from the DHT sensor continuously
  sensors.requestTemperatures(); 
  sensorData.temperatureDS18B20 = sensors.getTempCByIndex(0);

  // If reading was successful and the flag is set to send data
  if (!isnan(sensorData.temperatureDS18B20)) {
    if (sendDataFlag) {
      // Send the sensor data via ESP-NOW
      esp_now_send(receiverMAC, (uint8_t*)&sensorData, sizeof(sensorData));
      Serial.println("Data Sent: ");
      Serial.print("Temperature: ");
      Serial.print(sensorData.temperatureDS18B20);

      sendDataFlag = false; // Reset the flag after sending data
    }
  } else {
    Serial.println("Failed to read");
  }

  delay(2000); // Delay between sensor reads
}
