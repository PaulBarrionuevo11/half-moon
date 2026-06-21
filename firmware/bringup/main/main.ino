/* WIFI ICON*/
byte wifiIcon[8] = {
  0b00001, // .....
  0b00001, // ****.
  0b00101, // ..*..
  0b00101, // ..*..
  0b10101, // .....
  0b10101, // .***.
  0b10101, // .***.
  0b10101  // .....
};
byte crossIcon[8] = {
  0b00000, // .***.
  0b10001, // .....
  0b01010, // ****.
  0b00100, // ..*..
  0b00100, // ..*..
  0b01010, // .....
  0b10001, // .***.
  0b00000  // .....
};

#include<stdint.h>

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

bool wifiStatus = true;
bool sensorStatus = true;

#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
// Wi-Fi credentials
// Server URL
const char[]* ssid;
const char[]* password;
// const char* serverUrl = "http://10.0.0.110:5000"; //Testing on local server

//NTP Client DateTime
#define UTC_OFFSET_IN_SECONDS -25200  // offset from greenwich time
const char daysOfWeek[7][12] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "us.pool.ntp.org", UTC_OFFSET_IN_SECONDS, 6000);


#include <DHT.h>
#define DHTPIN 23 // Pin number for temp/hum sensor
#define DHTTYPE DHT22  
DHT dht(DHTPIN, DHTTYPE);
int32_t temp;
int32_t hum;

#include <Adafruit_TSL2561_U.h>
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
// TODO: Based decision of integration time
// on time of the day and year (more light vs dim)

void wifiConnectionStatus()
{
  int32_t counter = 0;
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
      delay(500);
      Serial.print(".");
      counter ++;
      if(counter == 20)
      {
        Serial.print("No WIFI connection established");
        wifiStatus = false;
        break;
      }
  }
  Serial.println("Connected to Wi-Fi");
}

void sensorConnectionStatus()
{
  dht.begin();
  temp = dht.readTemperature();
  hum = dht.readHumidity();
  if (isnan(temp) || isnan(hum))
  {
    Serial.println("Error while reading sensors");
    sensorStatus = false;
  }

  if (!tsl.begin())
  {
    Serial.println("Error while reading sensors");
    sensorStatus = false;
  }
  configureSensor();
  Serial.println("Sensors working well");
}

void configureSensor(void)
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
  Serial.println("------------------------------------");
  Serial.print  ("Gain:         "); Serial.println("Auto");
  Serial.print  ("Timing:       "); Serial.println("13 ms");
  Serial.println("------------------------------------");
}

void setup() {
  Serial.begin(115200);
  timeClient.begin();

  lcd.init();
  lcd.backlight();
  lcd.createChar(0, wifiIcon);
  lcd.createChar(1, crossIcon);

  lcd.setCursor(1,0);
  lcd.print("------------------");
  lcd.setCursor(1,1);
  lcd.print("Half Moon Station");
  lcd.setCursor(1,2);
  lcd.print("------------------");
  delay(2000);
  lcd.clear();
  lcd.setCursor(1,1);
  lcd.print("Updating Status ...");

  wifiConnectionStatus();
  sensorConnectionStatus();
  if(wifiStatus == true && sensorStatus == true)
  {
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("Update Completed");
    Serial.println("Wifi and Sensors are OK");

  }
  else if(wifiStatus == false)
  {
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("Updated with Error");
    delay(2000);
    Serial.println("Wifi response was not good");
  }
  else if(sensorStatus == false)
  {
    lcd.clear();
    lcd.setCursor(1,2);
    lcd.print("Updated with Error");
    delay(2000);
    Serial.println("Sensors response was not good");
  }
  lcd.clear();
}

void loop() {

  // Display DateTime information
  timeClient.update();
  Serial.println(daysOfWeek[timeClient.getDay()]);
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());

  if(wifiStatus == false)
  {
    lcd.setCursor(19,0);
    lcd.write(1);
  }
  else if (wifiStatus == true)
  {
    lcd.setCursor(19,0);
    lcd.write(0); // Full icon  

  }
  lcd.setCursor(6, 0);
  lcd.print("MY ROOM");
  lcd.setCursor(0, 1);
  lcd.print("Temp:");
  lcd.setCursor(7, 1);
  if(sensorStatus == false)
  {
    lcd.print("ERROR");
  }
  else 
  {
    lcd.print(temp);
    lcd.print(" C");
  }
  lcd.setCursor(0, 2);
  lcd.print("Humid:");
  lcd.setCursor(7, 2);
  if (sensorStatus == false) 
  {
    lcd.print("ERROR");
  } 
  else 
  {
    lcd.print(hum);
    lcd.print(" %");
  }

  // GET ROOM LUMINOSITY
  /* Get a new sensor event */ 
  sensors_event_t event;
  tsl.getEvent(&event);
  /* Display the results (light is measured in lux) */
  if (event.light)
  {
    Serial.print(event.light); Serial.println(" lux");
    delay(2000);
  }
  lcd.setCursor(0, 3);
  lcd.print("Lux:");
  lcd.setCursor(7, 3);
  lcd.print(event.light);

}
