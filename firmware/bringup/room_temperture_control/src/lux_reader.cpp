// #include <WiFi.h>

// const char* ssid_STA     = "Dhamma25";       // Router SSID
// const char* password_STA = "Bonfire2025$$";

// const char* ssid_AP      = "hmb-01";       // AP name others see
// const char* password_AP  = "123456789";         // AP password

// void setup() {
//   Serial.begin(115200);
  
//   // Crucial: Set mode to BOTH AP and STA
//   WiFi.mode(WIFI_AP_STA);
  
//   // Start Access Point
//   WiFi.softAP(ssid_AP, password_AP);
//   Serial.print("AP started - IP: ");
//   Serial.println(WiFi.softAPIP());               // Usually 192.168.4.1
  
//   // Connect to home WiFi (Station mode)
//   Serial.print("Connecting to ");
//   Serial.println(ssid_STA);
//   WiFi.begin(ssid_STA, password_STA);
  
//   int attempts = 0;
//   while (WiFi.status() != WL_CONNECTED && attempts < 20) {
//     delay(500);
//     Serial.print(".");
//     attempts++;
//   }
  
//   if (WiFi.status() == WL_CONNECTED) {
//     Serial.println("\nConnected to home WiFi!");
//     Serial.print("Station IP: ");
//     Serial.println(WiFi.localIP());              // Router-assigned IP
//   } else {
//     Serial.println("\nFailed to connect to home WiFi - AP mode only");
//   }
// }

// void loop() {
//   // Example: Check status periodically
//   if (WiFi.status() == WL_CONNECTED) {
//     // You have internet
//   }
//   delay(10000);
// }