/*
 
  Baseline IOT_Hme_Weather_1007
  
  NPT
    https://RandomNerdTutorials.com/esp8266-nodemcu-date-time-ntp-client-server-arduino/
  
  
*/

/*************************************************************************************
 * Libraries
*************************************************************************************/
#define _DISABLE_TLS_           //Thinger
#include <WiFi.h>                                             // Wifi for ESP32
#include <ThingerESP32.h>                                     // Thinger
#include "time.h"


/*************************************************************************************
 * Global definitions
*************************************************************************************/

const char *ssid     = "CasaDoTheodoro";                      // Wifi
const char *password = "09012011";
                                            
String wifiSSID;                                              // getWifiStatus()
String wifiIP;                                                // getWifiStatus()
int wifiRSSI;                                                 // getWifiStatus()

String currentTime;                                           // NPTrequest()
String currentDate;                                           // NPTrequest()
String currentFullDate;                                       // NPTrequest()

String ino = __FILE__;                                        // getSWversion()



#define USERNAME "fpuhl"
#define DEVICE_ID "IOT_HOME_CAM_dev"
#define DEVICE_CREDENTIAL "QUucfMJ?uE0+hlyQ"
ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

/*************************************************************************************
 * Setup
*************************************************************************************/
void setup() {
  
  Serial.begin(115200);                                       // Initialize Serial Monitor
    delay(3000);                                              // Delay to stabilize serial com
  configTime(-10800, 0, "pool.ntp.org");                      // NPT
  
  getSWversion();
  wifiConnect();                                              // Connect to Wi-Fi
  getWifiStatus();
  NPTrequest();
  ThingerDefinitions();
  
}

void loop() {
  
  getWifiStatus();
  NPTrequest();
  thing.handle();
  
  delay(2000);
}
