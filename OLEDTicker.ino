
/**************************************************
    getting ticker info from coinmarketcap.com
 **************************************************/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

//------- Install From Library Manager -------
#include <ArduinoJson.h>
#include <CoinMarketCapApi.h>
#include <Adafruit_NeoPixel.h>
//--------------------------------------------

//------- OLED display -------
#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

//  UNO connections -> use hardware I2C connections
//  SDA ->A4
//  SCL ->A5

//  ESP8266 module ->use hardware I2C connections
//  SDA ->NodeMCU D2 pin
//  SCL ->NodeMCU D1 pin

/*
  U8glib  Overview:
    Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
    Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
    U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.
*/

// U8g2 Contructor List (Frame Buffer)
// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
//--------------------------------------------


char ssid[] = "TELUS0183";       // SSID
char password[] = "6z5g4hbdxi";  // your network key

#define PIN 9            //Neopixel Data Pin  [ESP8266 - GPIO9]
#define NUM_LEDS 6       //Length of Neopixel Strand

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

// LED Definitions
const int LED_GREEN = 16; //ESP8266 - GPIO16
const int LED_RED = 5;    //ESP8266 - GPIO5
const int LED_BLUE = 4;   //ESP8266 - GPIO4
int i, j = 0;

// CoinMarketCap's limit is "no more than 10 per minute"
unsigned long api_mtbs = 10000; //mean time between api requests - original 60000
unsigned long api_due_time = 0;

WiFiClientSecure client;
CoinMarketCapApi api(client);


void setup() {

  Serial.begin(115200);


  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  u8g2.begin();
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr);  // choose a suitable font
  u8g2.drawStr(0, 10, "Connecting"); // write something to the internal memory
  u8g2.sendBuffer();          // transfer internal memory to the display

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    //Serial.print(".");
    //delay(500);'
    colorWipe(strip.Color(0, 0, 255), 500); // Blue
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  u8g2.clearBuffer();               // clear the internal memory
  u8g2.drawStr(0, 10, "Connected"); // write to the internal memory
  u8g2.sendBuffer();                // transfer internal memory to the display

  ConnectedWipe();
  strip.begin();
  strip.show();
}

void printTickerData(String ticker) {
  Serial.println("---------------------------------");
  //Serial.println("Getting ticker data for " + ticker);

  CMCTickerResponse response = api.GetTickerInfo(ticker);
  if (response.error == "") {
    Serial.print("Name: ");
    Serial.println(response.name);
    Serial.print("Symbol: ");
    Serial.println(response.symbol);

    Serial.print("Price in USD: ");
    Serial.println(response.price_usd);


    u8g2.clearBuffer();          // clear the internal memory, requires sendBuffer
    u8g2.sendBuffer();          // transfer internal memory to the display
        
    // u8g2.drawStr(0,30,"hello");  // write something to the internal memory

    u8g2.setCursor(0, 10);
    u8g2.print(ticker);


    u8g2.setCursor(50, 10);
    u8g2.print(response.price_usd);

    u8g2.setCursor(0, 25);
    u8g2.print("1hr");
    
    u8g2.setCursor(0, 40);
    u8g2.print(response.percent_change_1h);

    u8g2.setCursor(30, 25);
    u8g2.print("24hr");
    
    u8g2.setCursor(30, 40);
    u8g2.print(response.percent_change_24h);   


    u8g2.setCursor(70, 25);
    u8g2.print("7d");
    
    u8g2.setCursor(70, 40);
    u8g2.print(response.percent_change_7d);   

    

    u8g2.sendBuffer();          // transfer internal memory to the display

    //Serial.print("24h Volume USD: ");
    //Serial.println(response.volume_usd_24h);
    //Serial.print("Market Cap USD: ");
    //Serial.println(response.market_cap_usd);

    //Serial.print("Percent Change 1h: ");
    //Serial.println(response.percent_change_1h);
    Serial.print("Percent Change 24h: ");
    Serial.println(response.percent_change_24h);
    Serial.print("Percent Change 7d: ");
    Serial.println(response.percent_change_7d);
    Serial.print("Last Updated: ");
    Serial.println(response.last_updated);

  } else {
    Serial.print("Error getting data: ");
    Serial.println(response.error);
  }
  Serial.println("---------------------------------");

  if ((response.percent_change_24h) > 0) {
    digitalWrite(LED_GREEN, LOW); //Green active low
    digitalWrite(LED_RED, HIGH);
    strip.clear();
    if ((response.percent_change_24h) < 5) {
      strip.setPixelColor(3, strip.Color(0, 255, 0));
      strip.show();
    }
    else if (((response.percent_change_24h) > 5) && (response.percent_change_24h) < 10) {
      strip.setPixelColor(3, strip.Color(0, 255, 0));
      strip.setPixelColor(4, strip.Color(0, 255, 0));
      strip.show();
    }
    else if ((response.percent_change_24h) > 10) {
      strip.setPixelColor(3, strip.Color(0, 255, 0));
      strip.setPixelColor(4, strip.Color(0, 255, 0));
      strip.setPixelColor(5, strip.Color(0, 255, 0));
      strip.show();
    }
  }
  else if ((response.percent_change_24h) < 0) {
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED, LOW);
    strip.clear();
    if ((response.percent_change_24h) > -5) {
      strip.setPixelColor(2, strip.Color(255, 0, 0));
      strip.show();
    }
    else if (((response.percent_change_24h) < -5) && (response.percent_change_24h) > -10) {
      strip.setPixelColor(2, strip.Color(255, 0, 0));
      strip.setPixelColor(1, strip.Color(255, 0, 0));
      strip.show();
    }
    else if ((response.percent_change_24h) < -10) {
      strip.setPixelColor(2, strip.Color(255, 0, 0));
      strip.setPixelColor(1, strip.Color(255, 0, 0));
      strip.setPixelColor(0, strip.Color(255, 0, 0));
      strip.show();
    }
  }

}

void loop() {
  unsigned long timeNow = millis();

  if ((timeNow > api_due_time))  {
    printTickerData("bitcoin");
    //printTickerData("ethereum");
    // printTickerData("dogecoin");
    api_due_time = timeNow + api_mtbs;
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  //for(uint16_t i=0; i<strip.numPixels(); i++) {
  Serial.print(i);
  strip.setPixelColor(i, c);
  strip.show();
  delay(wait);
  i++;
  if (i > strip.numPixels()) {
    strip.clear();
    i = 0;
  }
}

void ConnectedWipe() {
  i = 0;
  j = 0;
  for (uint16_t j = 0; j < 3; j++)
  {
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(255, 255, 255));
      strip.show();
    }
    delay(300);
    //strip.clear();
    //strip.show();
    //delay(100);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
