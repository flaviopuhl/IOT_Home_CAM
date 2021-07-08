/*********
  Flavio Puhl

  IOT_HME_CAM  (KISS methodology)
  
  Board "AI Thinker ESP32-CAM"
  
  Content:
    Take pic + save to SD card ............. 102
    Wifi + NPT  ............................ 200 

Take Pic
  https://randomnerdtutorials.com/esp32-cam-take-photo-save-microsd-card/

*/

/***************************************************************************************
**                          Libraries
***************************************************************************************/

#include "esp_camera.h"
#include "Arduino.h"
#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
#include <EEPROM.h>            // read and write from flash memory

#include <WiFi.h>
#include "time.h"
/***************************************************************************************
**                          User configurations
***************************************************************************************/

// define the number of bytes you want to access
#define EEPROM_SIZE 1

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

/***************************************************************************************
**                         Global variable declaration
***************************************************************************************/

int pictureNumber = 0;

unsigned long previousMillis_1 = 0;        // will store last time weather was updated
const long interval_1 = 5*30*1000;         // interval at which update happens (milliseconds)

const char* ssid       = "CasaDoTheodoro";
const char* password   = "09012011";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -10800;//3600;
const int   daylightOffset_sec = 0;//3600;
String CurrentTime;

/***************************************************************************************
**                         Pre-Setup
***************************************************************************************/


/***************************************************************************************
**                         Setup
***************************************************************************************/

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
 
  Serial.begin(115200);
  delay(2000);

  Serial.println();

        String ino = __FILE__;                                                    // Extract file name for version tracking
        ino = (ino.substring((ino.indexOf(".")), (ino.lastIndexOf("\\")) + 1));
          Serial.println(ino);
  
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } 

        //connect to WiFi
            Serial.printf("Connecting to %s ", ssid);
            WiFi.begin(ssid, password);
            while (WiFi.status() != WL_CONNECTED) {
                delay(500);
                Serial.print(".");
            }
            Serial.println(" CONNECTED");
  
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  
  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  } else {
    Serial.println("Camera initialized");
  }
  
  //Serial.println("Starting SD Card");
  if(!SD_MMC.begin()){
    Serial.println("SD Card Mount Failed");
    return;
  } else {
    Serial.println("SD Card Mounted");
  }
  
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD Card attached");
    return;
  } else {
    Serial.println("SD Card attached");
  }
    

}

/***************************************************************************************
**                         Main Loop
***************************************************************************************/

void loop() {

  unsigned long currentMillis = millis();

    if (currentMillis - previousMillis_1 >= interval_1) {
          previousMillis_1 = currentMillis; 
          printLocalTime();
          takeApicture();        

     }
}

/***************************************************************************************
**                  Take a picture and save on SD Card
***************************************************************************************/
void takeApicture(){
                  
                  camera_fb_t * fb = NULL;
                  
                  // Take Picture with Camera
                  fb = esp_camera_fb_get();  
                  if(!fb) {
                    Serial.println("Camera capture failed");
                    return;
                  }
                  // initialize EEPROM with predefined size
                  EEPROM.begin(EEPROM_SIZE);
                  pictureNumber = EEPROM.read(0) + 1;
                
                  // Path where new picture will be saved in SD Card
                  //String path = "/picture" + String(pictureNumber) +".jpg";
                      String path = "/picture " + CurrentTime +".jpg";
               
                  fs::FS &fs = SD_MMC; 
                  Serial.printf("Picture file name: %s\n", path.c_str());
                  
                  File file = fs.open(path.c_str(), FILE_WRITE);
                  if(!file){
                    Serial.println("Failed to open file in writing mode");
                  } 
                  else {
                    file.write(fb->buf, fb->len); // payload (image), payload length
                    Serial.printf("Saved file to path: %s\n", path.c_str());
                    EEPROM.write(0, pictureNumber);
                    EEPROM.commit();
                  }
                  file.close();
                  esp_camera_fb_return(fb); 
                  
                  // Turns off the ESP32-CAM white on-board LED (flash) connected to GPIO 4
                  //pinMode(4, OUTPUT);
                  //digitalWrite(4, LOW);
                  //rtc_gpio_hold_en(GPIO_NUM_4);
                  //digitalWrite(4, HIGH);
                  
                  delay(500);
                  Serial.println("Pic Taken");
}

/***************************************************************************************
**                      NPT
***************************************************************************************/

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
 
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  
  char timeHour[3];
  char timeMin[3];
  char timeSec[3];
  char timeDay[3];
  char timeMonth[3];
  char timeYear[3];
  strftime(timeHour,3, "%H", &timeinfo);
  strftime(timeMin,3, "%M", &timeinfo);
  strftime(timeSec,3, "%S", &timeinfo);
  strftime(timeDay,3, "%d", &timeinfo);
  strftime(timeMonth,3, "%m", &timeinfo);
  strftime(timeYear,3, "%Y", &timeinfo);

  CurrentTime = String(timeDay)+String(timeMonth)+String(timeYear)+" "+String(timeHour)+String(timeMin)+String(timeSec);
  
  Serial.println(CurrentTime);

  Serial.println();
}
