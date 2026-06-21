#include "WifiService.h"

WifiService::WifiService()
{

}
void WifiService::begin()
{
    int32_t counter = 0;

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    if(WiFi.status() != WL_CONNECTED)
    {
        delay(800);
        Serial.print(".");
        counter ++;
        if(counter == 20)
        {
            Serial.print("No WIFI connection established");
            // LED SERVICE
        }
      }
  
        Serial.println("Connected to Wi-Fi");
        Serial.print("ESP32 Local IP: ");
        Serial.println(WiFi.localIP());
}

void WifiService::disconnect()
{

}