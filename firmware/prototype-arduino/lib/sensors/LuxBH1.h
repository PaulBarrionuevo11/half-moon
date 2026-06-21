#pragma once

#include <Arduino.h>
#include "Wire.h"

class LuxBH1
{
    public:
        LuxBH1();
        void begin();
        float readLuxLevel();
        bool sensorConnected(); // Check if sensor is connected and internal temperature

    private:

		float lux = 0.0;

        const uint8_t BH1750_ADDRESS = 0x23;
		const uint8_t bytes_requested = 6;

        // ESP32 pins
        const uint8_t sda_pin = 23;
        const uint8_t scl_pin = 22;

        const uint32_t clock_speed = 100000;
	

        // Internal helper
        bool readSensor();
    

};