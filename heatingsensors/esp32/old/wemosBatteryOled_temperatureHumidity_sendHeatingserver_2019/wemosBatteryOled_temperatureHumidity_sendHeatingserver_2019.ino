
// Oled wemos will not compile or upload with sensor connected !!!!!!!



#include "DHT.h"
#define DHTTYPE DHT11 
const int DHTPin = 2; 
DHT dht(DHTPin, DHTTYPE);
float temperature10MinutesAgo;
int secondsCounter = 0;

#include <WiFi.h>
#include "arduino_secrets.h"
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;


const char* url = "http://192.168.0.37";
const int port = 4200;

#include <HTTPClient.h>


int freq = 1700;
int channel = 0;
int resolution = 8;

void setup() {
  Serial.begin(9600);
  
  dht.begin();

  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  // buzzer
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(0, channel);



}
void loop() {

  delay(1000);

    
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
    }
  else {
    Serial.println("Temperature = "+String(t));
    Serial.println("Humidity = "+String(h));
  }


  if (secondsCounter % (60 * 10 ) == 0){ // save temperature every 10 minutes 

      if (temperature10MinutesAgo != t ) {
          HTTPClient http;   
          http.begin("http://192.168.0.37:8080/api/sensorvalue/");  
          http.addHeader("Content-Type", "application/json");
          http.POST("{\"temperature\":"+String(t)+",\"humidity\":"+String(h)+"}");
          http.writeToStream(&Serial);
          http.end();      
      }
    
      if (temperature10MinutesAgo - t > 4 && t < 22) { // alarm if window open
        ledcWriteTone(channel, 1700); 
        delay(120000);
      }
      secondsCounter = 0;
      temperature10MinutesAgo = t;
  }
  
  secondsCounter++;


   
}
