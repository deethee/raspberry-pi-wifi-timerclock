//3.3 Volt

#include "DHT.h"
#define DHTTYPE DHT11 
const int DHTPin = 4; 
DHT dht(DHTPIN, DHTTYPE);



// Temporary variables
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];


#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "UPC9406652";
const char* password = "300873170473";

const char* url = "http://192.168.0.37";
const int port = 4200;




// based on an orginal sketch by Arduino forum member "danigom"
// http://forum.arduino.cc/index.php?action=profile;u=188950
 

#include "LedControl.h"
 
const int numDevices = 1;      // number of MAX7219s used
const long scrollDelay = 75;   // adjust scrolling speed
 
unsigned long bufferLong [14] = {0}; 
 
LedControl lc=LedControl(2,0,4,numDevices);
 
prog_uchar scrollText[] PROGMEM ={
    "  THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG 1234567890 the quick brown fox jumped over the lazy dog   \0"};


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



  //LED
    for (int x=0; x<numDevices; x++){
        lc.shutdown(x,false);       //The MAX72XX is in power-saving mode on startup
        lc.setIntensity(x,8);       // Set the brightness to default value
        lc.clearDisplay(x);         // and clear the display
    }
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

    
  HTTPClient http;
  /*
  http.begin("http://192.168.0.37:8080/api/sensortemperature/");
  http.addHeader("Content-Type", "application/json");
  http.POST("11.1");
  http.writeToStream(&Serial);
  http.end();
*/
/* Firewall deaktivieren
    http.begin("http://192.168.0.2:8080/api/sensorvalue/");
*/
    http.begin("http://192.168.0.37:8080/api/sensorvalue/");
    
    http.addHeader("Content-Type", "application/json");
    http.POST("{\"temperature\":"+String(t)+",\"humidity\":"+String(h)+"}");
    http.writeToStream(&Serial);
    http.end();  
    delay(15 * 60 * 1000);
}
   
}


 




