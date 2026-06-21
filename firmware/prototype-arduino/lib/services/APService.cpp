#include "APService.h"

APService::APService()
{}

void APService::begin()
{
    ap_status = WiFi.softAP(ap_name, ap_password);

    if(ap_status == WL_IDLE_STATUS)
    {
        Serial.printf("Connection in progress: enum %i", ap_status);
    }
    else if (ap_status != WL_CONNECTED)
    {
        Serial.printf("Connection failed: enum %i", ap_status);
    }
    else
    {
        Serial.printf("AP succesfully created: enum %i \n", ap_status);
        Serial.printf("Actively broadcasting");
        ip_address = WiFi.softAPIP();
        Serial.println("IP address: ");
        Serial.println(ip_address);
    }
}

void APService::stop()
{
    WiFi.softAPdisconnect();

    if(ap_status == WL_DISCONNECTED)
    {
        Serial.printf("Succesfully disconnected: enum %i", ap_status);
    }
}