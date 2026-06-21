#include <WiFi.h>
#include <HTTPClient.h>
#include <stdint.h>
#include <ArduinoJson.h> // Use ArduinoJson for JSON serialization

// Wi-Fi credentials
const char* ssid = "Dhamma";
const char* password = "Bonfire2025$$";
const char* serverUrl = "http://10.0.0.110:5000/";  // Replace with your Flask server's IP and port

void setup() {
  Serial.begin(115200);

    // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

}
void loop() {
  // You can add logic here to send messages periodically or on events (e.g., button press)
  delay(5000);  // Example: Send every 5 seconds
  sendMessage("Periodic message from ESP32");
}

void sendMessage(const char* message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    // Create JSON payload
    StaticJsonDocument<200> doc;
    doc["message"] = message;
    String jsonPayload;
    serializeJson(doc, jsonPayload);

    int httpResponseCode = http.POST(jsonPayload);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);  // Print server's response (e.g., {"status": "success", "received": "Hello from ESP32!"})
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}
