#include "SHT41.h"

/*
	If a previously installed I2C bus or device is no longer needed, 
	it's recommended to recycle the resource by calling 
	i2c_master_bus_rm_device() or i2c_del_master_bus()

	I2C bus is configured in begin() and sensorConnected()
*/

// TODO: confirm if i2c_device_config_t can be configured once and just be called by 
//			i2c_master_bus_add_device() and i2c_master_transmit_receive()

SHT41::SHT41()
{

}

void SHT41::begin()
{

	Wire.begin(sda_pin, scl_pin);
    Wire.setClock(clock_speed);
	
}

bool SHT41::sensorConnected()
{
    Wire.beginTransmission(SHT41_ADDRESS);

    if (Wire.endTransmission() == 0)
    {
        return true;
    }

    return false;
}

bool SHT41::readSensor()
{
    Wire.beginTransmission(SHT41_ADDRESS);
    Wire.write(0xFD); // High precision measurement

	// Sends everything over the bus 
    if (Wire.endTransmission() != 0)
    {
        return false;
    }

    delay(10);

    Wire.requestFrom(SHT41_ADDRESS, bytes_requested );

    if (Wire.available() != 6)
    {
		/*
		sensor disconnected
		bad wiring
		wrong address
		no pullups
		sensor powered off
		*/
        return false;
    }

    uint16_t raw_temp = (Wire.read() << 8) | Wire.read();
    Wire.read(); // CRC byte

    uint16_t raw_humidity = (Wire.read() << 8) | Wire.read();
    Wire.read(); // CRC byte

    temperature = -45 + 175 * ((float)raw_temp / 65535.0);
    humidity = -6 + 125 * ((float)raw_humidity / 65535.0);

    return true;
}

float SHT41::getTemperature()
{
    readSensor();
    return temperature;
}

float SHT41::getHumidity()
{
    readSensor();
    return humidity;
}
