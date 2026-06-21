#include <Arduino.h>
#include "RGBLed.h"
#include "SHT41.h"

RGBLed rgb_led(12, 14, 27) ;
SHT41 sht41;

uint8_t red[3] = {255, 0, 0};
uint8_t green[3] = {0, 255, 0};
uint8_t blue[3] = {0, 0, 255};
uint8_t white[3] = {220, 220, 220};

float temp = 0.0;
float hum = 0.0;


void setup() {
  // put your setup code here, to run once:   
  rgb_led.begin();
  sht41.begin();
}

void loop() {
  rgb_led.setColor(blue, 0.10);
  temp = sht41.getTemperature();
  hum = sht41.getHumidity();

  float mcu_temp;

  Serial.printf("Current temperature %f and humidity %f", temp, hum);
  Serial.printf("Current temperature of the MCU %f", mcu_temp);

}
