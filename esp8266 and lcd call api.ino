//code scrapped together by Nevin Danielson nevindanielson@gmail.com
//TODO:refactor so lcdDisplay function isn't so repetitive
//TODO:figure out why I can't move the http stuff into a separate function. When I try and make a GetPayload() function it breaks
//TODO: how to call a void function? I'm calling an int and returning a 1
//TODO:

#include <ESP8266HTTPClient.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <LiquidCrystal.h>

//mapping specific to ESP8266
#define d0 3
#define d1 1
#define d2 16
#define d3 5
#define d4 4
#define d5 14
#define d6 12
#define d7 13
#define d8 0
#define d9 2
#define d10 15

//declare global variables
int timestamp;
String payload;

LiquidCrystal lcd(d8,d9,d4,d5,d6,d7);

ESP8266WiFiMulti WiFiMulti;

int lcdDisplay(String payload)
 
//instead of using a parser for JSON, I'm just searching for keywords in the String
//the numbers change, but the keywords stay the same. I can find the numbers from those anchors.

        {
        int tempTag = payload.indexOf("temp");
        //after I find the index of the keyword, I know where the needed number starts:
        int tempStart = tempTag+6;
        //and roughly where it ends
        int tempEnd = tempTag+8;
         
        //take the char after the last index
        char tempChar = payload[tempEnd+1];

        //and see if it is a digit or not
        while (isDigit(tempChar))
          {
            //if it is a digit, add it to the range of the desired index
            tempEnd = tempEnd+1;
            //and prepare to test the next one
            tempChar = payload[tempEnd+1];        
          }
        // now that I know the end, capture it as a String
        String tempInString = payload.substring(tempStart, tempEnd+1);
        
        int feelTag = payload.indexOf("feels_like");
        int feelStart = feelTag+12;
        int feelEnd = feelTag+14;
        String feelInString;
        char feelChar = payload[feelEnd+1];
        while (isDigit(feelChar))
          {
            feelEnd = feelEnd+1;
            feelChar = payload[feelEnd+1];        
          }
        feelInString = payload.substring(feelStart, feelEnd+1);
        
        lcd.setCursor(0, 0);
        lcd.print("temp: " + tempInString);
        lcd.setCursor(0, 1);
        lcd.print("feels like: " + feelInString);
        delay(2000);



        int minTag = payload.indexOf("temp_min");
        int minStart = minTag+10;
        int minEnd = minTag+12;
        String minInString;
        char minChar = payload[minEnd+1];
        while (isDigit(minChar))
          {
            minEnd = minEnd+1;
            minChar = payload[minEnd+1];        
          }
        lcd.clear();
        minInString = payload.substring(minStart, minEnd+1);
       
        int maxTag = payload.indexOf("temp_max");
        int maxStart = maxTag+10;
        int maxEnd = maxTag+12;
        String maxInString;
        char maxChar = payload[maxEnd+1];
        while (isDigit(maxChar))
          {
            maxEnd = maxEnd+1;
            maxChar = payload[maxEnd+1];        
          }
        lcd.clear();
        maxInString = payload.substring(maxStart, maxEnd+1);
       
        lcd.setCursor(0, 0);
        lcd.print("min: " + minInString);
        lcd.setCursor(0, 1);
        lcd.print("max: " + maxInString);
        delay(2000);

        return 1;
        }

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println();
  lcd.begin(16, 2);
  timestamp = millis();
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("YOUR_SSID", "YOUR_PASSWORD");

if ((WiFiMulti.run() == WL_CONNECTED)) 
  {
    WiFiClient client;
    HTTPClient http;
    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, "http://api.openweathermap.org/data/2.5/weather?q=Regina&units=metric&appid=YOUR_API_KEY")) 
    { 
      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();
      // httpCode will be negative on error
      if (httpCode > 0) 
      {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      }
     // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) 
      {
        Serial.println("returning GetPayload function");
         payload=http.getString();
      }
    }
  }
  int call=lcdDisplay(payload);
}

void loop() 
{
  if (millis() > timestamp+70000) //don't exceed the free call amount at openweathermap.org
  {
   if ((WiFiMulti.run() == WL_CONNECTED)) 
  {
    WiFiClient client;
    HTTPClient http;
    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, "http://api.openweathermap.org/data/2.5/weather?q=Regina&units=metric&appid=54cc82d379192b5fd110a47129f46a56")) 
    { 
      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();
      // httpCode will be negative on error
      if (httpCode > 0) 
      {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      }
     // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) 
      {
        Serial.println("returning GetPayload function");
         payload=http.getString();
      }
    }
  }
   timestamp=millis();
 }
 int call=lcdDisplay(payload);
}
        


