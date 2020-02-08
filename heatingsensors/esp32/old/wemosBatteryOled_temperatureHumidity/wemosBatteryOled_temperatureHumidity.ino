//3.3 Volt

#include "DHT.h"
#define DHTTYPE DHT11 
const int DHTPin = 2; 
DHT dht(DHTPin, DHTTYPE);



// Temporary variables
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];




#include <WiFi.h>
#include "arduino_secrets.h"
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;

const char* url = "http://192.168.0.37";
const int port = 4200;



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

}
void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
    }
  else {
    Serial.println();
    Serial.println("Temperature = "+String(t));
    Serial.println("Humidity = "+String(h));

}



   
}
