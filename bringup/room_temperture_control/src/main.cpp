/**
 * Halfmoon Sensor monitoring test
 */
// I2C pins on ESP32 SDA 21 - SCL 22
#include <stdint.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <Adafruit_TSL2561_U.h>
#include <Arduino.h>
#include "esp_adc_cal.h"  // For calibration

#define   DHTPIN    (5) // Pin number for temp/hum sensor
#define   DHTTYPE   (DHT22)  
const char* SSID = "Dhamma25";
const char* PASSWORD = "Bonfire2025$$";
const char* SERVER_URL = "http://10.0.0.110:5000/ingest";
const char* DEVICE_ID = "HM-01";

float tempC = 0;
float humid = 0;
float batteryV = 0;
const float dividerRatio = 10.0 / (30.0 + 10.0);  // ≈0.1667
int samples = 50;

bool wifi_status = false;
bool sensorStatus = false;

// 1 minutes
constexpr unsigned long PERIOD_MS = 1UL * 60UL * 1000000UL;
unsigned long lastSend = 0;

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
DHT dht(DHTPIN, DHTTYPE);
sensors_event_t light;

enum Battery_State
{
  BATTERY_OK = 0,
  BATTERY_LOW,
  BATTERY_CRITICAL,
  BATTERY_CHARGING,
  BATTERY_ERROR,
  BATTERY_IDLE
};

enum System_State
{
  BOOT = 0,
  LOGGING,
  SYNCING,
  SLEEP,
  ERROR,
};

enum DHT_State
{
  DHT_INIT,
  DHT_OK,
  DHT_ERROR
};

enum Lux_State
{
  LUX_INIT,
  LUX_OK,
  LUX_ERROR
};

enum WiFi_State
{
  WIFI_OK,
  WIFI_ERROR
};

// Enum initialization
enum Battery_State batteryState = BATTERY_IDLE;
enum System_State systemState = BOOT;
enum DHT_State dhtState;
enum Lux_State luxState;


/** Function prototypes **/ 
/// SYSTEM CONFIGURATION ///
void WifiConnection();
void ConfigureLuxSensor(void);
void DHTConnection();
void BatteryReading();

/// LOGGING //
void LogInit(System_State systemState, Battery_State batteryState);
void LogSystem(System_State systemState, Battery_State batteryState, DHT_State dhtState, Lux_State luxState);
void LogData(const char* device_id, bool wifi_status, float tempC, float humid, sensors_event_t lux, float batteryV);

/// Send Logs to Server ///
bool PostTelemetry(float tempC, float humidity, float light, float batteryV);
bool PostInitLog();
bool PostSystemLog();


void setup() {
  Serial.begin(115200);
  dht.begin();
  LogInit(systemState, batteryState);

  WifiConnection();
  ConfigureLuxSensor();
  DHTConnection();
  BatteryReading();  

  systemState = LOGGING;
  
  // Initial logging BOOT -> LOGGING // BOOT -> ERROR
  LogSystem(systemState, batteryState,dhtState, luxState);

  // Set wake-up source
  esp_sleep_enable_timer_wakeup(PERIOD_MS);
  // Prepare for sleep
  systemState = SLEEP;
  LogData(DEVICE_ID, wifi_status, tempC, humid, light, batteryV);
  Serial.flush();
  // Enable sleep
  esp_deep_sleep_start();
}

 void loop() {
    // unsigned long now = millis();
    // if (now - lastSend >= PERIOD_MS || lastSend == 0) {
    //   lastSend = now;
    //   tempC = dht.readTemperature();
    //   humid = dht.readHumidity();
    //   // Serial.print("Temp in C:");
    //   // Serial.println(tempC);
    //   // Serial.print("Humid in RH %:");
    //   // Serial.println(humid);
  
    //   // tsl.getEvent(&light);
    //   // Serial.print("Luminosity: ");
    //   // Serial.print(light.light); 
    //   // Serial.println(" lux");

    //   BatteryReading();

    //   systemState = LOGGING;
    //   Logs(DEVICE_ID, systemState, wifi_status, tempC, humid, light, batteryState);

    //   if (!PostTelemetry(tempC, humid, light.light, batteryV)) {
    //     Serial.println("Failed to send telemetry.");
    //     // Logs(DEVICE_ID, systemState, wifi_status, tempC, humid, light, batteryState);
    //   }
    // }
    // delay(50);
    
}
void LogInit(System_State, Battery_State)
{
  Serial.printf("LogInit | system=%d battery=%d \n", systemState, batteryState);
}
void LogSystem(System_State systemState, Battery_State batteryState, DHT_State dhtState, Lux_State luxState)
{
  Serial.printf("LogSystem | system=%d battery=%d dht=%d lux=%d \n", systemState, batteryState,dhtState, luxState);
}

void LogData(const char* device_id, bool wifi_status, float tempC, float humid, sensors_event_t lux, float batteryV)
{
  Serial.printf("LogData | id=%s wifi=%d temp=%0.2fC humid=%0.2f lux=%0.2f batteryV=%0.2fV \n", device_id, wifi_status, tempC, humid, lux.light, batteryV);
  systemState = SYNCING;
  PostTelemetry(tempC, humid, lux.light, batteryV);
}
void WifiConnection()
{
  int32_t counter = 0;
  // Connect to Wi-Fi
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) 
  {
      delay(1000);
      Serial.print(".");
      counter ++;
      if(counter == 20)
      {
        Serial.print("No WIFI connection established");
        wifi_status = false;
      }
  }
  wifi_status = true;
  Serial.println("Connected to Wi-Fi");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());
}
void ConfigureLuxSensor(void)
{
  Lux_State::LUX_INIT; 
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  
  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */  
  Serial.println("------------------------------------");
  Serial.print  ("Gain:         "); Serial.println("Auto");
  Serial.print  ("Timing:       "); Serial.println("13 ms");
  Serial.println("------------------------------------");

  if(!tsl.getEvent(&light))
    DHT_State::DHT_ERROR;

  else
    tsl.getEvent(&light);
    Lux_State::LUX_OK; 
}
void DHTConnection()
{
  dht.begin();
  tempC = dht.readTemperature();
  humid = dht.readHumidity();

  if (isnan(tempC) || isnan(humid))
    DHT_State::DHT_ERROR;

  else
    DHT_State::DHT_OK;
}
void BatteryReading()
{
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(34);
    delay(1);
  }
  int raw = sum / samples;
  // Calibrated voltage reading (accounts for chip variations)
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
  uint32_t voltage_mv = esp_adc_cal_raw_to_voltage(raw, &adc_chars);
  float voltageAtPin = voltage_mv / 1000.0;
  batteryV = voltageAtPin / dividerRatio;

    // LOGS STATUS
  if(batteryV > 9.2)   // ADJUST
    batteryState = BATTERY_OK;
  else if(batteryV > 7.0 && batteryV < 9.2)
    batteryState = BATTERY_LOW;
  else if(batteryV < 7.0 && batteryV > 6.2)
    batteryState = BATTERY_CRITICAL;
  else
    batteryState = BATTERY_ERROR;

  // Serial.print("Raw: ");
  // Serial.print(raw);
  // Serial.print(" | Voltage at Pin: ");
  // Serial.print(voltageAtPin, 3);
  // Serial.print("V | Battery Voltage: ");
  // Serial.print(batteryV, 3);  // 3 decimals for precision
  // Serial.println("V");
}
bool PostTelemetry(float tempC, float humidity, float light, float batteryV) {
  if (WiFi.status() != WL_CONNECTED) wifi_status = false;

  HTTPClient http;
  http.begin(SERVER_URL);
  http.addHeader("Content-Type", "application/json");

  int rssi = WiFi.RSSI();

  // Build JSON payload
  String body = "{";
  body += "\"device_id\":\"" + String(DEVICE_ID) + "\",";
  body += "\"temp_c\":" + String(tempC, 2) + ",";
  body += "\"humidity\":" + String(humidity, 2) + ",";
  body += "\"luminosity\":" + String(light, 2) + ",";
  body += "\"battery_v\":" + String(batteryV, 2) + ",";
  body += "\"rssi\":" + String(rssi);
  body += "}";

  int code = http.POST(body);
  String resp = http.getString();
  http.end();

  Serial.printf("POST %s -> code=%d resp=%s\n", SERVER_URL, code, resp.c_str());
  return (code >= 200 && code < 300);
}