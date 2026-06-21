#pragma once

#include <stdint.h>

/*
Things it can report
    internal MCU temperature
    free heap memory
    uptime
    reset reason
    WiFi signal strength
    chip model
    CPU frequency
*/

struct McuStatus {
  float internalTempC;
  uint32_t freeHeap;
  int wifiRSSI;
  uint32_t uptimeSeconds;
};