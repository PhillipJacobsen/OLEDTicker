
/**************************************************
 *  getting ticker info from coinmarketcap.com    *
 **************************************************/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

//------- Install From Library Manager -------
#include <ArduinoJson.h>
#include <CoinMarketCapApi.h>
#include <Adafruit_NeoPixel.h>
//--------------------------------------------


char ssid[] = "TELUS2358";       // SSID
char password[] = "";  // your network key

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
int i,j=0;

// CoinMarketCap's limit is "no more than 10 per minute"
unsigned long api_mtbs = 60000; //mean time between api requests
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
   else if((response.percent_change_24h) < 0) {
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
    if (i>strip.numPixels()) {
      strip.clear();
      i=0;
    } 
}

void ConnectedWipe() {
     i=0;
     j=0;  
    for(uint16_t j=0; j<3; j++) 
    {
        for(uint16_t i=0; i<strip.numPixels(); i++) {
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
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
