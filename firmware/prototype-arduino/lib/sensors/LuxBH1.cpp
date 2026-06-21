#include "LuxBH1.h"

LuxBH1::LuxBH1()
{}

void LuxBH1::begin()
{
    Wire.begin();
}

bool LuxBH1::sensorConnected()
{
    Wire.beginTransmission(BH1750_ADDRESS);
    return Wire.endTransmission() == 0;
}

bool LuxBH1::readSensor()
{
    // Power on
    Wire.beginTransmission(BH1750_ADDRESS);
    Wire.write(0x01);

    if (Wire.endTransmission() != 0)
    {
        return false;
    }

    // Continuous high resolution mode
    Wire.beginTransmission(BH1750_ADDRESS);
    Wire.write(0x10);

    if (Wire.endTransmission() != 0)
    {
        return false;
    }

    delay(180);

    uint8_t bytes_read = Wire.requestFrom(BH1750_ADDRESS, (uint8_t)2);

    if (bytes_read != 2)
    {
        return false;
    }

    uint16_t raw_lux = (Wire.read() << 8) | Wire.read();

    lux = raw_lux / 1.2;

    return true;
}

float LuxBH1::readLuxLevel()
{
    if (readSensor())
    {
        return lux;
    }

    return -1.0; // error value
}