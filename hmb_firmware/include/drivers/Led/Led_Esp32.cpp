#include "Led_Esp32.h"
#include "Arduino.h"

Led_Esp32::Led_Esp32()
{
    /* Set PIN for ESP32 */
    gpio_config_t io_pin0_config = {
        .pin_bit_mask = (1UL << LED_BUILTIN),      // Select GPIO 5 for RED
        .mode = GPIO_MODE_OUTPUT,               // Set as output
        .pull_up_en = GPIO_PULLUP_DISABLE,      // Disable pull-up
        .pull_down_en = GPIO_PULLDOWN_DISABLE,  // Disable pull-down
        .intr_type = GPIO_INTR_DISABLE             // Disable interrupts
    };
    gpio_config_t io_pin1_config = {
        .pin_bit_mask = (1UL << LED_RED),       // Select GPIO 5 for RED
        .mode = GPIO_MODE_OUTPUT,               // Set as output
        .pull_up_en = GPIO_PULLUP_DISABLE,      // Disable pull-up
        .pull_down_en = GPIO_PULLDOWN_DISABLE,  // Disable pull-down
        .intr_type = GPIO_INTR_DISABLE          // Disable interrupts
    };
};  

void Led_Esp32::toggle_led(gpio_num_t _pin_number)
{
    for (int i=0; i<3; i++)
    {
        delay(1500);
        gpio_set_level(_pin_number, LED_ON);     
        delay(1500);
        gpio_set_level(_pin_number, LED_OFF);
        delay(1500);
        gpio_set_level(_pin_number, LED_ON);
        delay(1500);
        gpio_set_level(_pin_number, LED_OFF);
    }
}  

void Led_Esp32::Led_Enable(gpio_num_t _pin_number)
{
    gpio_set_level(_pin_number, LED_ON);     
}
void Led_Esp32::Led_Disable(gpio_num_t _pin_number)
{
    gpio_set_level(_pin_number, LED_OFF);     
}
