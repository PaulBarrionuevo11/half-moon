#pragma once

#include <Arduino.h>
#include "Wire.h"


class SHT41
{
	public:
		// Specific I2C pins for ESP32-C3 are data: GPIO8, clock: GPIO9
		SHT41();
		void begin();
		float getTemperature();
		float getHumidity();
		bool sensorConnected(); // Check if sensor is connected and internal temperature

	private:
		float temperature = 0.0;
		float humidity = 0.0;

        const uint8_t SHT41_ADDRESS = 0x44;
		const uint8_t bytes_requested = 6;

        // ESP32 pins
        const uint8_t sda_pin = 23;
        const uint8_t scl_pin = 22;

        const uint32_t clock_speed = 100000;
	

        // Internal helper
        bool readSensor();

};