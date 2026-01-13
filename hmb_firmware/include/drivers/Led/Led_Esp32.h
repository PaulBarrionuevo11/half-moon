#pragma once

#include "driver/gpio.h"

#define LED_RED         (5)
#define LED_BUILTIN     (2)
#define LED_ON          (1)      
#define LED_OFF         (0)      


class Led_Esp32
{

    public:
        Led_Esp32();
        ~Led_Esp32();
        void toggle_led(gpio_num_t _pin_number);         
        void Led_Enable(gpio_num_t _pin_number);
        void Led_Disable(gpio_num_t _pin_number);

};
