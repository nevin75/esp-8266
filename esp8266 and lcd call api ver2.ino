//code scrapped together by Nevin Danielson nevindanielson@gmail.com

#include <ESP8266HTTPClient.h>
#include <Arduino.h>
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
String payload = "";
bool debug=false;
bool debugGetIndex=false;
bool debugGetString=false;

LiquidCrystal lcd(d8,d9,d4,d5,d6,d7);

ESP8266WiFiMulti WiFiMulti;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println();
  lcd.begin(16, 2);
  timestamp = millis();
  for (uint8_t t = 4; t > 0; t--) {
    if (debug)  Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("[YOUR_SSID", "YOUR_PASSWORD");
  pinMode(3, OUTPUT);
}

void loop() 
{
  if (millis() > timestamp+70000) //delay our calls so we don't exceed the free call amount at openweathermap.org
  {
     payload = GetPayload(WiFiMulti);
     timestamp=millis();
  }

  if (payload == "") {
     payload = GetPayload(WiFiMulti);
     timestamp=millis();
  }
  int indexForTagword = (payload.indexOf("temp"));
  String tempAsAString=GetString(payload.indexOf("temp"));
  String feelAsAString=GetString(payload.indexOf("feels_like"));
  String maxAsAString=GetString(payload.indexOf("temp_max"));
  String minAsAString=GetString(payload.indexOf("temp_min"));
  DisplayLCD(minAsAString, maxAsAString, tempAsAString, feelAsAString);
  ToggleColdWarning (feelAsAString);
  delay(3000);
}

String GetPayload (ESP8266WiFiMulti WiFiMulti)
 {
  if ((WiFiMulti.run() == WL_CONNECTED)) 
  {
    WiFiClient client;
    HTTPClient http;
    if (debug)    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, "http://api.openweathermap.org/data/2.5/weather?q=Regina&units=metric&appid=[YOUR_API_KEY]")) 
    { 
        if (debug)    Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();
      // httpCode will be negative on error
      if (httpCode > 0) 
      {
        // HTTP header has been send and Server response header has been handled
          if (debug)    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      }
     // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) 
      {
        return http.getString();
      }
    }
  }
  return "";
}

int ReturnDistanceFromTagOfFirstDigit(int startingPosition)
{
  int currentPosition = startingPosition;
  while (!((isDigit(payload.charAt(currentPosition)))||((payload.charAt(currentPosition)==('-'))))){
    currentPosition++;
    }
  int distance = (currentPosition-startingPosition);
  return distance;
}

int ReturnDistanceFromStartOfNumber(int startingPosition)
{
  int currentPosition = startingPosition+1;
  while (isDigit(payload.charAt(currentPosition))||(payload.charAt(currentPosition)=='.')){
    currentPosition++;
    }
  int distance = (currentPosition-startingPosition);
  return distance;
}

String GetString(int indexForTagword)
{
  int distanceFromTag = ReturnDistanceFromTagOfFirstDigit(indexForTagword);
  int startOfNumberIndex = (indexForTagword + distanceFromTag);
  int distanceFromStart = ReturnDistanceFromStartOfNumber(startOfNumberIndex);
  int endOfNumberIndex = (startOfNumberIndex + distanceFromStart);
  return payload.substring(startOfNumberIndex, endOfNumberIndex);
}

void DisplayLCD(String minLine, String maxLine, String tempLine, String feelLine)
{ 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("min: " + minLine);
  lcd.setCursor(0, 1);
  lcd.print(" max: " + maxLine);
  delay (3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("temp: " + tempLine);
  lcd.setCursor(0, 1);
  lcd.print(" feels: " + feelLine);
}

//test "feels_like" and turn on warning LED
void ToggleColdWarning (String feelString)
{
  if (feelString.toDouble()<=-25)   digitalWrite (3, HIGH);
  else                              digitalWrite(3, LOW);
}
