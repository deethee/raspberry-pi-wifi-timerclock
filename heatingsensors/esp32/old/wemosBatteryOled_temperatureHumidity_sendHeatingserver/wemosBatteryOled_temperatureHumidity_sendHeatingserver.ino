#include <HTTPClient.h>
#include <WiFi.h>

#include "arduino_secrets.h"
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;




#include "DHT.h"
// Uncomment one of the lines below for whatever DHT sensor type you're using!
#define DHTTYPE DHT11   // DHT sensor type 11
// DHT Sensor
const int DHTPin = 1035;
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

// Temporary variables
static char celsiusTemp[7];
static char humidityTemp[7];


//Wemos xring
#include <Adafruit_NeoPixel.h> // https://github.com/adafruit/Adafruit_NeoPixel
#define PIN 0 
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, PIN, NEO_GRB + NEO_KHZ800);



void setup() {
  // initialize the DHT sensor
  dht.begin();
  
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while(!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  // attempt to connect to Wifi network:
  while(WiFi.status() != WL_CONNECTED) {
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());




  // init xring
 strip.begin();
 for(int i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(random(0, 255), random(0, 255), random(0, 255)));
      strip.setBrightness(random(0, 100));
      strip.show();
      delay(random(0, 1000));
 }  
 for(int i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(255, 0, 0));
      strip.setBrightness(5);
      strip.show();
      delay(0);
  }
  delay(2000);
  // end init xring
}

void loop() {
   
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

           // Check if any reads failed and exit early (to try again).
            if (isnan(h) || isnan(t) ) {
              Serial.println("Failed to read from DHT sensor!");
              strcpy(celsiusTemp,"Failed");
              strcpy(humidityTemp, "Failed");       
                
               for(int i=0; i<strip.numPixels(); i++) {
                    strip.setPixelColor(i, strip.Color(0, 255, 0));
                    strip.setBrightness(3);
                    strip.show();
                    delay(0);
              }
            }
            else{
              // Computes temperature values in Celsius + Fahrenheit and Humidity
              float hic = dht.computeHeatIndex(t, h, false);       
              dtostrf(hic, 6, 2, celsiusTemp);                   
              dtostrf(h, 6, 2, humidityTemp);

              Serial.println("temp:"+String(t) + ", humidity:"+String(h));
              
              HTTPClient http;   
              http.begin("http://192.168.0.37:8080/api/sensorvalue/");
              
              http.addHeader("Content-Type", "application/json");
              http.POST("{\"temperature\":"+String(t)+",\"humidity\":"+String(h)+"}");
              http.writeToStream(&Serial);
              http.end();         

              int redLed=0;
              int blueLed=0;
              boolean activateLed=false;
              if (h > 65.0 ){
                redLed=255 * h;
                activateLed = true;
                Serial.println("humidity too high");
              }

              if (t < 16.0 ){
                blueLed=255 * t/16;
                activateLed = true;
                Serial.println("temperature too low");

              }
              if (activateLed){
                strip.Color(redLed, 0, blueLed);
                strip.setBrightness(30);
                Serial.println("temperature too low");
              }else{
                strip.Color(0, 255, 0);
                strip.setBrightness(5);
                Serial.println("temperature humidity ok");
              }
          }


  delay(1000*10*1);
 // delay(1000*60*15);
}
