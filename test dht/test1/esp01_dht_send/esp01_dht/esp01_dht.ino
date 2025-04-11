#include <ESP8266WiFi.h>
#include <espnow.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

uint8_t receiverMAC[] = { 0x2C, 0x3A, 0xe8, 0x25, 0xdb, 0x4c };  // Replace with the receiver's MAC address

typedef struct {
  float temperature;
  float humidity;
} SensorData;

SensorData sensorData;

void setup() {
  Serial.begin(115200);
  dht.begin();
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init() == 0) {
    Serial.println("ESP-NOW Initialized");
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    esp_now_add_peer(receiverMAC, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
    esp_now_register_send_cb([](uint8_t* mac, uint8_t sendStatus) {
      Serial.print("Send status: ");
      Serial.println(sendStatus == 0 ? "Success" : "Failed");
    });
  } else {
    Serial.println("ESP-NOW Initialization Failed");
    ESP.restart();
  }
}

void loop() {
  sensorData.temperature = dht.readTemperature();
  sensorData.humidity = dht.readHumidity();
  esp_now_send(receiverMAC, (uint8_t*)&sensorData, sizeof(sensorData));
  delay(2000);
}