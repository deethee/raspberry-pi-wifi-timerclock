#include <dummy.h>

/*
 * Rui Santos 
 * Complete Project Details http://randomnerdtutorials.com
*/

#include <HTTPClient.h>
#include <WiFi.h>
// Replace with your network credentials
const char* ssid     = "UPC9406652";
const char* password = "300873170473";


#include "DHT.h"
// Uncomment one of the lines below for whatever DHT sensor type you're using!
#define DHTTYPE DHT11   // DHT sensor type 11
// DHT Sensor
const int DHTPin = 4;
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

// Temporary variables
static char celsiusTemp[7];
static char humidityTemp[7];

/*
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_SSD1306.h"
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);
*/ 




void setup() {
  // initialize the DHT sensor
  dht.begin();
  
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while(!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

/*
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  display.display();
*/  
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
  

}

void loop() {
   
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

/*
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
*/  
            // Check if any reads failed and exit early (to try again).
            if (isnan(h) || isnan(t) ) {
              Serial.println("Failed to read from DHT sensor!");
              strcpy(celsiusTemp,"Failed");
              strcpy(humidityTemp, "Failed");         
            }
            else{
              // Computes temperature values in Celsius + Fahrenheit and Humidity
              float hic = dht.computeHeatIndex(t, h, false);       
              dtostrf(hic, 6, 2, celsiusTemp);                   
              dtostrf(h, 6, 2, humidityTemp);

              Serial.println("temp:"+String(t) + ", humidity:"+String(h));
              
              /*// Temperature in Celcius
              display.println("Temp.");
              //display.print(t);
              display.println(" c");
              // Humidity in % 
              display.println("Humidity");
              //display.print(h);
              //display.println(" %");
              display.display();
              */

              HTTPClient http;   
              http.begin("http://192.168.0.37:8080/api/sensorvalue/");
              
              http.addHeader("Content-Type", "application/json");
              http.POST("{\"temperature\":"+String(t)+",\"humidity\":"+String(h)+"}");
              http.writeToStream(&Serial);
              http.end();  
             
    
          }


  delay(1000*60*15);

}
