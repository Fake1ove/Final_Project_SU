#include <ESP8266WiFi.h>
#include <espnow.h>

typedef struct struct_message {
  float temperature;
  float humidity;
} struct_message;

struct_message receivedData;


void dataReceived(uint8_t *senderMac, uint8_t *incomingData, uint8_t len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));
  Serial.print("Temperature: ");
  Serial.print(receivedData.temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(receivedData.humidity);
  Serial.println(" %");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init() == 0) {
    Serial.println("ESP-NOW Initialized.");
    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
    esp_now_register_recv_cb(dataReceived);
  } else {
    Serial.println("ESP-NOW Initialization Failed.");
    ESP.restart();
  }
}

void loop() {
  
}