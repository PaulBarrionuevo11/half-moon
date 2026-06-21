#pragma once

#include <Arduino.h>
#include "driver/ledc.h"

class RGBLed {
public:
    // ESP32-C3 specific default pins (e.g., GPIO 3, 4, 5)
    RGBLed(int pinR, int pinG, int pinB);
    void begin();
    void setColor(uint8_t colors[3], float brightness);

private:
    int _pins[3];
    ledc_channel_t _channels[3];

};

