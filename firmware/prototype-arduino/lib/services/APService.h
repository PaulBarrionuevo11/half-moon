#pragma once

#include "WiFi.h"

class APService
{
    public:
    APService();
    void begin();
    void stop();

    private:
    String ap_name = "HalfMoon - V1";
    String ap_password = "lightup123";
    IPAddress ip_address;
    uint8_t ap_status;
};