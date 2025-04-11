#include <ESP8266WiFi.h>
#include <espnow.h>

// REPLACE WITH RECEIVER MAC Addresses
uint8_t receiverMac1[] = {0x5c, 0xcf, 0x7f, 0x06, 0xf4, 0xc2}; // ตัวรับที่ 1
uint8_t receiverMac2[] = {0x5c, 0xcf, 0x7f, 0x85, 0x50, 0x17}; // ตัวรับที่ 2

// Structure example to send data
typedef struct test_struct {
    int x;
    int y;
} test_struct;

test_struct test;

unsigned long lastTime = 0;  
unsigned long timerDelay = 2000;  // send readings timer

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  char macStr[18];
  Serial.print("Packet to:");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
         mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
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
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    // Set values to send
    test.x = 0;
    test.y = 0;

    // Send message to Receiver 1
    esp_now_send(receiverMac1, (uint8_t *) &test, sizeof(test));
    Serial.println("Data sent to Receiver 1");

    // Send message to Receiver 2
    esp_now_send(receiverMac2, (uint8_t *) &test, sizeof(test));
    Serial.println("Data sent to Receiver 2");

    lastTime = millis();
  }
}
