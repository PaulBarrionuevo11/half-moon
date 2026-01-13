#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "driver/gpio.h"
#include "esp_rom_sys.h"      // esp_rom_delay_us
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"

#define DHT_PIN GPIO_NUM_15   // change to your pin

class Dht22_Esp32
{
private:
    /* data */
public:
    Dht22_Esp32(/* args */);
    ~Dht22_Esp32();
};

Dht22_Esp32::Dht22_Esp32(/* args */)
{
}

Dht22_Esp32::~Dht22_Esp32()
{
}

// Wait for pin to become "level" with timeout in microseconds.
// Returns elapsed time if success, -1 if timeout.
static int wait_for_level(gpio_num_t pin, int level, int timeout_us)
{
    int t = 0;
    while (gpio_get_level(pin) != level) {
        if (t++ >= timeout_us) return -1;
        esp_rom_delay_us(1);
    }
    return t;
}

// Measure how long pin stays at "level" (in microseconds), with timeout.
// Returns duration or -1 if timeout.
static int measure_level_width(gpio_num_t pin, int level, int timeout_us)
{
    int t = 0;
    while (gpio_get_level(pin) == level) {
        if (t++ >= timeout_us) return -1;
        esp_rom_delay_us(1);
    }
    return t;
}

static void dht_set_output(gpio_num_t pin)
{
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
}

static void dht_set_input(gpio_num_t pin)
{
    gpio_set_direction(pin, GPIO_MODE_INPUT);
}

// Reads DHT22. Returns true on success. Outputs humidity (%) and temperature (C).
bool dht22_read(gpio_num_t pin, float *humidity, float *temperature)
{
    uint8_t data[5] = {0};

    // DHT needs a pull-up
    gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);

    // Timing-critical section
    static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&mux);

    // 1) Start signal
    dht_set_output(pin);
    gpio_set_level(pin, 1);
    esp_rom_delay_us(1000);

    gpio_set_level(pin, 0);
    // DHT22: >= 1ms, commonly 18ms for compatibility
    vTaskDelay(pdMS_TO_TICKS(18));

    gpio_set_level(pin, 1);
    esp_rom_delay_us(30);

    dht_set_input(pin);

    // 2) Sensor response: 80us low, 80us high
    if (wait_for_level(pin, 0, 100) < 0) { portEXIT_CRITICAL(&mux); return false; }
    if (wait_for_level(pin, 1, 100) < 0) { portEXIT_CRITICAL(&mux); return false; }

    // Now should go low to start bit transmission
    if (wait_for_level(pin, 0, 100) < 0) { portEXIT_CRITICAL(&mux); return false; }

    // 3) Read 40 bits
    for (int i = 0; i < 40; i++) {
        // Each bit starts with ~50us LOW
        int low_dur = measure_level_width(pin, 0, 100);
        if (low_dur < 0) { portEXIT_CRITICAL(&mux); return false; }

        // Then HIGH: length indicates 0/1
        int high_dur = measure_level_width(pin, 1, 120);
        if (high_dur < 0) { portEXIT_CRITICAL(&mux); return false; }

        // Threshold between 0 and 1 (tune if needed)
        int bit = (high_dur > 45) ? 1 : 0;

        data[i / 8] <<= 1;
        data[i / 8] |= bit;
    }

    portEXIT_CRITICAL(&mux);

    // 4) Verify checksum
    uint8_t sum = (uint8_t)(data[0] + data[1] + data[2] + data[3]);
    if (sum != data[4]) return false;

    // 5) Convert
    uint16_t rh = ((uint16_t)data[0] << 8) | data[1];
    uint16_t rt = ((uint16_t)data[2] << 8) | data[3];

    *humidity = rh / 10.0f;

    // Temp sign bit (DHT22)
    if (rt & 0x8000) {
        rt &= 0x7FFF;
        *temperature = -(rt / 10.0f);
    } else {
        *temperature = rt / 10.0f;
    }

    return true;
}
