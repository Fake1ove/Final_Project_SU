#include <ESP8266WiFi.h>
#include <espnow.h>

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);  // LED Off initially
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_now_init();
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(onDataReceived);
}

void loop() {
  // Nothing to do here, waiting for ESP-NOW messages
}

void onDataReceived(uint8_t *senderMac, uint8_t *data, uint8_t len) {
  if (*data == 1) {
    digitalWrite(LED_BUILTIN, LOW);  // Turn LED ON
    Serial.println("LED ON");
  } else if (*data == 0) {
    digitalWrite(LED_BUILTIN, HIGH);  // Turn LED OFF
    Serial.println("LED OFF");
  }
}
