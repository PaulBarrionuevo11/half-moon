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
#define   DHTTYPE   (DHT11)  
#define LED_BUILTIN 2 // 

const char* SSID = "MakerNexus";
const char* PASSWORD = "Maker234"; // IP at Marco's House == ESP32 IP: 192.168.86.30

const char* SERVER_URL = "http://10.0.0.110:5000/data";
const char* DEVICE_ID = "uniqueid-01";

float tempC = 0;
float humid = 0;
float batt_voltage = 0;
const float divider_ratio = 10.0 / (30.0 + 10.0);  // ≈0.1667
int samples = 50;

const int red_pin = 26;
const int green_pin = 25;
const int blue_pin = 27;
// 1 minutes
constexpr unsigned long PERIOD_MS = 1UL * 60UL * 1000000UL;

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
DHT dht(DHTPIN, DHTTYPE);
sensors_event_t light;

enum Battery_State
{
  BATTERY_ERROR = -1,
  BATTERY_OK = 0,
  BATTERY_IDLE = 1,
  BATTERY_LOW,
  BATTERY_CRITICAL,
  BATTERY_CHARGING,  
};

enum System_State
{
  SYS_ERROR = -1,
  SYS_BOOT = 0,
  SYS_IDLE = 1,
  SYS_LOGGING,
  SYS_SYNCING,
  SYS_SLEEP,
};

enum Sensors_State
{
  SENSORS_ERROR = -1,
  SENSORS_OK = 0,
  SENSORS_IDLE = 1,
};

enum WiFi_State
{
  WIFI_ERROR = -1,
  WIFI_OK = 0,
  WIFI_IDLE = 1,
};

// Enum initialization
enum Battery_State batt_state = BATTERY_IDLE;
enum System_State system_state = SYS_IDLE;
enum Sensors_State sensors_state = SENSORS_IDLE;
enum WiFi_State wifi_state = WIFI_IDLE;

/** Function prototypes **/ 
void WifiConnection();
void DHTConnection();
void ConfigureLuxSensor(void);
void BatteryReading();

/*
Functions that log states from the entire system.
1) LogSystem - will check the state of the components and record an initial status.
      Only implemented in the inital setup of the code. Repeated after coming bacj
      from sleep.
2) LogData - Logs device unique id as well as sensor data. This is the same as the 
      one sent to the server. Good for firmware debug.
3) PostTelemetry - Data sent to the server
*/
void LogSystem(System_State system_state, Battery_State batt_state, Sensors_State sensors_state);
void LogData(const char* device_id, float tempC, float humid, sensors_event_t lux, float batt_voltage);
bool PostTelemetry(float tempC, float humidity, float light, float batt_voltage);

void setup() {
  Serial.begin(115200);
  dht.begin();
  // pinMode(LED_BUILTIN, OUTPUT);
  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);

  // digitalWrite(LED_BUILTIN, HIGH); Turn LED DURING system check
  analogWrite(red_pin, 0);   
  analogWrite(green_pin, 255);   
  analogWrite(blue_pin, 0);
  delay(200);  // Wait 200 milliseconds  

  // TODO: Define comment with LED state based on this cofiguration state
  WifiConnection();
  DHTConnection();
  if(system_state != System_State::SYS_ERROR)
    ConfigureLuxSensor();
  BatteryReading();  

  system_state = SYS_LOGGING;
  LogSystem(system_state, batt_state, sensors_state);

  // Set wake-up source
  esp_sleep_enable_timer_wakeup(PERIOD_MS);
  // Prepare for sleep
  system_state = SYS_SLEEP;
  LogData(DEVICE_ID, tempC, humid, light, batt_voltage);

  Serial.flush();
  // Enable sleep
  // delay(2000); 
  analogWrite(red_pin, 0);
  analogWrite(green_pin, 0);
  analogWrite(blue_pin, 0);
  delay(800);  // Wait 1 second

  esp_deep_sleep_start();
}

void loop() {}

void WifiConnection()
{
  int32_t counter = 0;
  // Connect to Wi-Fi
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) 
  {
      delay(800);
      Serial.print(".");
      counter ++;
      if(counter == 20)
      {
        Serial.print("No WIFI connection established");
        wifi_state = WIFI_ERROR;
        for (int i=0; i < 4; i++)
        {
          analogWrite(red_pin, 255);
          analogWrite(green_pin, 0);
          analogWrite(blue_pin, 0);
          delay(1000);  // Wait 1 second
        }
        break;;
      }
  }
  Serial.println("Connected to Wi-Fi");
  Serial.print("ESP32 Local IP: ");
  Serial.println(WiFi.localIP());
  wifi_state = WIFI_OK;
}

void DHTConnection()
{
  dht.begin();
  tempC = dht.readTemperature();
  humid = dht.readHumidity();

  if (isnan(tempC) || isnan(humid))
  {
    Sensors_State::SENSORS_ERROR;
    tempC = -1;
    humid = -1;
    analogWrite(red_pin, 0);
    analogWrite(green_pin, 0);
    analogWrite(blue_pin, 225);
    delay(1000);
    analogWrite(blue_pin, 0);
  }
  Sensors_State::SENSORS_OK; 
}

void ConfigureLuxSensor(void)
{
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  
  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */  
  // Serial.println("------------------------------------");
  // Serial.print  ("Gain:         "); Serial.println("Auto");
  // Serial.print  ("Timing:       "); Serial.println("13 ms");
  // Serial.println("------------------------------------");

  if(!tsl.getEvent(&light))
  {
    Sensors_State::SENSORS_ERROR;
    analogWrite(red_pin, 100);
    analogWrite(green_pin, 0);
    analogWrite(blue_pin, 255);
  }
  else
    tsl.getEvent(&light);
    Sensors_State::SENSORS_OK;
}

void LogSystem(System_State sys_state, Battery_State batt_state, Sensors_State sensors_state)
{
  Serial.printf("LogSystem | system=%d battery=%d sensors=%d \n", sys_state, batt_state, sensors_state);
}

void LogData(const char* device_id, float tempC, float humid, sensors_event_t lux, float batt_voltage)
{
  Serial.printf("LogData | id=%s wifi=%d temp=%0.0f humid=%0.0f lux=%0.1f batteryV=%0.1f \n", device_id, tempC, humid, lux.light, batt_voltage);
  system_state = SYS_SYNCING;
  PostTelemetry(tempC, humid, lux.light, batt_voltage);
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
  batt_voltage = voltageAtPin / divider_ratio;

    // LOGS STATUS
  if(batt_voltage > 9.2)   // ADJUST
    Battery_State::BATTERY_OK;
  else if(batt_voltage > 7.0 && batt_voltage < 9.2)
    Battery_State::BATTERY_LOW;
  else if(batt_voltage < 7.0 && batt_voltage > 6.2)
    Battery_State::BATTERY_CRITICAL;
  else
    Battery_State::BATTERY_ERROR;

  // Serial.print("Raw: ");
  // Serial.print(raw);
  // Serial.print(" | Voltage at Pin: ");
  // Serial.print(voltageAtPin, 3);
  // Serial.print("V | Battery Voltage: ");
  // Serial.print(batteryV, 3);  // 3 decimals for precision
  // Serial.println("V");
}
bool PostTelemetry(float tempC, float humidity, float light, float batt_voltage) {
  if (WiFi.status() != WL_CONNECTED) WiFi_State::WIFI_ERROR;

  WiFi_State::WIFI_OK;
  HTTPClient http;
  http.begin(SERVER_URL);
  http.addHeader("Content-Type", "application/json");

  int rssi = WiFi.RSSI();

  // Build JSON payload
  String body = "{";
  body += "\"station_id\":\"" + String(DEVICE_ID) + "\",";
  body += "\"temperature\":" + String(tempC, 2) + ",";
  body += "\"humidity\":" + String(humidity, 2) + ",";
  body += "\"light\":" + String(light, 2) + ",";
  body += "\"battery\":" + String(batt_voltage, 2) + ",";
  body += "\"rssi\":" + String(rssi);
  body += "}";

  int code = http.POST(body);
  
  Serial.print("Response code: ");
  Serial.println(code);

  http.end();
  return (code >= 200 && code < 300);
}