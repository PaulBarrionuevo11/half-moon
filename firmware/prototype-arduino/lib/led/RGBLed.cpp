#include "RGBLed.h"

RGBLed::RGBLed(int pinR, int pinG, int pinB) {
    _pins[0] = pinR; _pins[1] = pinG; _pins[2] = pinB;
    _channels[0] = LEDC_CHANNEL_0;
    _channels[1] = LEDC_CHANNEL_1;
    _channels[2] = LEDC_CHANNEL_2;
}

void RGBLed::begin() {

    // 1. Configure the LEDC Timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE, // ESP32-C3 only supports Low Speed
        .duty_resolution  = LEDC_TIMER_8_BIT,    // 0-255 range
        .timer_num        = LEDC_TIMER_0,
        .freq_hz          = 5000,                // 5kHz PWM signal frequency
        .clk_cfg          = LEDC_AUTO_CLK
    };

    ledc_timer_config(&ledc_timer);

    // 2. Configure the 3 PWM Channels (R, G, B)
    for (int i = 0; i < 3; i++) {
        ledc_channel_config_t ledc_channel = {
            .gpio_num       = _pins[i],
            .speed_mode     = LEDC_LOW_SPEED_MODE,
            .channel        = _channels[i],
            .intr_type      = LEDC_INTR_DISABLE,
            .timer_sel      = LEDC_TIMER_0,
            .duty           = 0,
            .hpoint         = 0,
            // .sleep_mode     = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD // not be powered down during sleep.
        };
        ledc_channel_config(&ledc_channel);
    }
}

void RGBLed::setColor(uint8_t colors[3], float brightness) {
    for (int i = 0; i < 3; i++) {
        uint32_t duty = colors[i] * brightness;  // brightness: 0.0 to 1.0

        ledc_set_duty(LEDC_LOW_SPEED_MODE, _channels[i], duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, _channels[i]);
    }
}
