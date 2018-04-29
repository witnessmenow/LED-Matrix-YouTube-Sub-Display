/*******************************************************************
    Using a 64 * 32 RGB Matrix to display YT Subscriber stats
 *                                                                 *
    Written by Brian Lough
    https://www.youtube.com/channel/UCezJOfu7OtqGzd5xrP3q6WA
 *******************************************************************/

// ----------------------------
// Standard Libraries - Already Installed if you have ESP8266 set up
// ----------------------------

#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>


// ----------------------------
// Additional Libraries - each one of these will need to be installed.
// ----------------------------


#include <PxMatrix.h>
// The library for controlling the LED Matrix
// Needs to be manually downloaded and installed
// https://github.com/2dom/PxMatrix

#include <YoutubeApi.h>
// For fetching the stats from the YouTube API
// Search for "YouTube" in the Arduino Library manager
// https://github.com/witnessmenow/arduino-youtube-api

#include <ArduinoJson.h>
// This Sketch doesn't technically need this, but the YT library does so it must be installed.
// Search for "ArduinoJson" in the library manger
// https://github.com/bblanchon/ArduinoJson


//------- Replace the following! ------
char ssid[] = "SSID";       // your network SSID (name)
char password[] = "password";  // your network key
#define API_KEY "abcdefghijklmnopqrs_tuvwxyz123456789abc"  // your google apps API Token (see readme of YT API library)
#define CHANNEL_ID "UCezJOfu7OtqGzd5xrP3q6WA" // makes up the url of channel
//------------------------------

Ticker display_ticker;

WiFiClientSecure client;
YoutubeApi api(API_KEY, client);

unsigned long api_mtbs = 60000; //mean time between api requests
unsigned long api_lasttime = 60000;   //last time api request has been done

// Pins for LED MATRIX
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_OE 2
#define P_D 12
#define P_E 0

// PxMATRIX display(32,16,P_LAT, P_OE,P_A,P_B,P_C);
// PxMATRIX display(64,32,P_LAT, P_OE,P_A,P_B,P_C,P_D);
PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);

// Some standard colors
uint16_t myRED = display.color565(255, 0, 0);
uint16_t myGREEN = display.color565(0, 255, 0);
uint16_t myBLUE = display.color565(0, 0, 255);
uint16_t myWHITE = display.color565(255, 255, 255);
uint16_t myYELLOW = display.color565(255, 255, 0);
uint16_t myCYAN = display.color565(0, 255, 255);
uint16_t myMAGENTA = display.color565(255, 0, 255);
uint16_t myBLACK = display.color565(0, 0, 0);

uint16 myCOLORS[8] = {myRED, myGREEN, myBLUE, myWHITE, myYELLOW, myCYAN, myMAGENTA, myBLACK};

// Converted using the following site: http://www.rinkydinkelectronics.com/t_imageconverter565.php
uint16_t static youTubeBigger[] = {
  0x0000, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0010 (16) pixels
  0xF800, 0xF800, 0xF800, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0020 (32) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0030 (48) pixels
  0xF800, 0xF800, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0040 (64) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0050 (80) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800,   // 0x0060 (96) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0070 (112) pixels
  0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0080 (128) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0090 (144) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x00A0 (160) pixels
  0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x00B0 (176) pixels
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x00C0 (192) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x00D0 (208) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xF800, 0xF800,   // 0x00E0 (224) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF,   // 0x00F0 (240) pixels
  0xFFFF, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0100 (256) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0110 (272) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0120 (288) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0130 (304) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0xF800,   // 0x0140 (320) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x0000, 0x0000,   // 0x0150 (336) pixels
};


// ISR for display refresh
void display_updater()
{

  display.display(70);

}
void setup() {

  // Define your display layout here, e.g. 1/8 step
  display.begin(16);
  display.clearDisplay();
  display.setTextColor(myCYAN);
  display.setCursor(2, 0);
  display.print("Connecting");
  display.setTextColor(myMAGENTA);
  display.setCursor(2, 8);
  display.print("to the");
  display.setTextColor(myGREEN);
  display.setCursor(2, 16);
  display.print("WiFi!");

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);


  Serial.begin(9600);

  Serial.print("Pixel draw latency in us: ");
  unsigned long start_timer = micros();
  display.drawPixel(1, 1, 0);
  unsigned long delta_timer = micros() - start_timer;
  Serial.println(delta_timer);

  Serial.print("Display update latency in us: ");
  start_timer = micros();
  display.display(0);
  delta_timer = micros() - start_timer;
  Serial.println(delta_timer);

  display_ticker.attach(0.002, display_updater);
  yield();
  delay(1000);
  display.clearDisplay();
  displayName("BRIAN","LOUGH"); // seemed to crash when I used lower case letters at the bottom!
  delay(5000);
}

void drawYouTube(int x, int y)
{
  int counter = 0;
  for (int yy = 0; yy < 16; yy++)
  {
    for (int xx = 0; xx < 21; xx++)
    {
      display.drawPixel(xx + x , yy + y, youTubeBigger[counter]);
      counter++;
    }
  }
}

void displayText(String text, int yPos) {
  int16_t  x1, y1;
  uint16_t w, h;
  display.setTextSize(2);
  char charBuf[text.length() + 1];
  text.toCharArray(charBuf, text.length() + 1);
  display.getTextBounds(charBuf, 0, yPos, &x1, &y1, &w, &h);
  int startingX = 33 - (w / 2);
  if (startingX < 0) {
    display.setTextSize(1);
    display.getTextBounds(charBuf, 0, yPos, &x1, &y1, &w, &h);
    startingX = 33 - (w / 2);
  }
  display.setCursor(startingX, yPos);
  Serial.println(startingX);
  Serial.println(yPos);
  display.print(text);
}

void updateScreen(long subCount) {
  display.clearDisplay();
  drawYouTube(21, 0);
  display.setTextColor(myGREEN);
  displayText(String(subCount), 17);
}

void displayName(String first, String last)
{
  display.setTextColor(myCYAN);
  displayText(first, 1);
  display.setTextColor(myMAGENTA);
  displayText(last, 17);
}

void loop() {
  if (millis() - api_lasttime > api_mtbs)  {
    if (api.getChannelStatistics(CHANNEL_ID))
    {
      Serial.println("---------Stats---------");
      Serial.print("Subscriber Count: ");
      Serial.println(api.channelStats.subscriberCount);
      updateScreen(api.channelStats.subscriberCount);
    }
    api_lasttime = millis();
  }
}
