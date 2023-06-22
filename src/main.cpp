#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <SPI.h>
#include <ArduinoJson.h>

// char ssid[16] = "ZTE_2.4G_C9dX7t";
// char passwordWifi[9] = "eNUfH2sc";

char ssid[16] = "ZTE_2.4G_C9dX7t";
char passwordWifi[9] = "eNUfH2sc";

char request[41] = "http://192.168.1.2:8080/command/device/1";
char logRequest[30] = "http://192.168.1.2:8080/log/1";
char accountRequest[30] = "http://192.168.1.2:8080/login";
char timestamp[25];
HTTPClient http;
WiFiClient client;
String data;
DynamicJsonDocument doc(512);
String token;
int command;
String timeData;


void setup() {

  // put your setup code here, to run once:
    Serial.begin(115200);
  
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, passwordWifi);
    http.begin(client,request);
    Wire.begin();

}

void loop() {

  http.setURL(request);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", token);
  int status = http.GET();

  if(status != HTTP_CODE_UNAUTHORIZED){
    if (status == HTTP_CODE_OK) {
      
      data = http.getString();
      if (data != "null" && data != "") {
        
        deserializeJson(doc, data.c_str());
        command = doc["command"];
        
        Wire.beginTransmission(8);
        Wire.write(command);
        delay(100);
        Wire.endTransmission();
        http.setURL(request);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("Authorization", token);
        http.DELETE();
        
        if(command == 0){
          Wire.requestFrom(8,21);
          doc["watt"]=Wire.read();

          timeData = "";

          for (int i = 0; i < 19; i++) {
            timeData += String(char(Wire.read()));
          }

          doc["time"]=timeData;
          doc["volt"] = 5;
          http.setURL(logRequest);
          http.addHeader("Content-Type", "application/json");
          http.addHeader("Authorization", token);
          data = "";
          serializeJson(doc,data);
          Serial.println(timeData);
          http.POST(data);
        }
      }
    }
  }else{
    http.setURL(accountRequest);
    http.addHeader("Content-Type","application/json");
    doc["username"]="something@gmail.com";
    doc["password"]="Asdf123!@#";
    serializeJson(doc,data);
    http.POST(data);
    data = http.getString();
    deserializeJson(doc,data);
    token = "Bearer " + doc["token"].as<String>(); // Concatenate string literal and String object
  }
}