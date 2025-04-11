#include <ESP8266WiFi.h>
#include <espnow.h>

// REPLACE WITH RECEIVER MAC Address
uint8_t receiverMac[] = {0x84, 0x0d, 0x8e, 0x97, 0xa0, 0xe3};  // MAC address ของตัวรับ

// Structure example to send data
typedef struct test_struct {
    int x;
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
  
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Transmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer with the receiver's MAC address
  esp_now_add_peer(receiverMac, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop() {
  if (Serial.available() > 0) {
    // Read the input from Serial Monitor
    char input = Serial.read();  

    // Check if the input is '1' or '0'
    if (input == '1') {
      test.x = 1;  // Set x to 1 if '1' is typed
    } else if (input == '0') {
      test.x = 0;  // Set x to 0 if '0' is typed
    }

    // Send the data via ESP-NOW
    esp_now_send(0, (uint8_t *) &test, sizeof(test));

    // Wait a moment before allowing more inputs
    delay(1000);  // Optional, to avoid continuous sending
  }
}
