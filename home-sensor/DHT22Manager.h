#pragma once

/*

Hardware Setup: Connect DHT22 VCC to 3.3V/5V, GND to GND, and Data pin to an ESP32 GPIO (e.g., GPIO 15/D15), often with a 4.7k-10k pull-up resistor on the data line.
Initialization:
Set the data pin as an OUTPUT.
Pull the pin HIGH (wait briefly).
Pull the pin LOW for at least 18ms to start the sensor.
Set the pin as INPUT to listen for the sensor's response.
Sensor Response (Listen):
Wait for the sensor to pull the line LOW (approx. 80µs).
Wait for the sensor to pull the line HIGH (approx. 80µs).
Read Data Bits (40 times):
For each bit, wait for the line to go LOW (approx. 50µs).
Check if the line goes HIGH (read ~26-28µs for '0' or ~70µs for '1').
Store the bit in your data buffer.
Parse Data: Extract humidity and temperature values from the 40 bits, check the checksum, and convert to float.
Repeat: Wait at least 2 seconds before the next read. 

*/

#include <stdint.h>

class DHT22Manager //: public GPIOManager
{
    public:
    DHT22Manager(uint32_t pin);

    bool begin();
    float readTemperature();
    float readHumidity();

    private:
    uint32_t _pin;

};