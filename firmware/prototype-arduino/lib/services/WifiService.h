#pragma once

#include "WiFi.h"

class WifiService
{

public:
    WifiService();

    void begin();
    void disconnect();

private:
    const char* ssid = "";
    const char* password = "";
};
