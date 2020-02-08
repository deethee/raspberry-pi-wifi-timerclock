

// Oled wemos will not compile or upload with sensor connected !!!!!!!

#include "DHT.h"
#define DHTTYPE DHT11 
const int DHTPin = 2; 
DHT dht(DHTPin, DHTTYPE);



#include <WiFi.h>
#include "arduino_secrets.h"
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;


const char* url = "http://192.168.0.37";
const int port = 4200;

#include <HTTPClient.h>

// buzzer
int freq = 1700;
int channel = 0;
int resolution = 8;


// oled
#include "SSD1306.h"
uint8_t ledPin = 16; // Onboard LED reference
uint8_t oledPin1 = 4; // Onboard LED reference
uint8_t oledPin2 = 5; // Onboard LED reference
SSD1306 display(0x3c, 5, 4); // instance for the OLED. Addr, SDA, SCL


// deep sleep
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  600        /* Time ESP32 will go to sleep (in seconds) */
RTC_DATA_ATTR float lastTemperature;
#include "driver/adc.h"

//Battery Voltage

int batteryPin = 13;// is the gpio pin not analog
float batteryVoltage = 0; // not working esp32 measures up to 3.3v needs voltage devider


void setup() {
  Serial.begin(9600);
 
  dht.begin();
  pinMode(batteryPin, INPUT);
  
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


  // oled
  display.init(); // initialise the OLED
  //display.flipScreenVertically(); // does what is says
  display.setFont(ArialMT_Plain_24); // does what is says
  // Set the origin of text to top left
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);



  // wait for sensors initialize
  delay(5000);

  int sensorValue = analogRead(batteryPin);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 3.3V):
  batteryVoltage = sensorValue * (3.3 / 4095.0);


  // following was loop
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
    t = -1;
    h = -1;
    } else {
    Serial.println("Temperature = "+String(t));
    Serial.println("Humidity = "+String(h));
    HTTPClient http;   
    http.begin("http://192.168.0.37:8080/api/sensorvalue/");  
    http.addHeader("Content-Type", "application/json");
    http.POST("{\"temperature\":"+String(t)+",\"humidity\":"+String(h)+"}");
    http.writeToStream(&Serial);
    http.end();    
  }

  
  // Upwards scroll OLED text
  for(int8_t n = 112; n > 0; n--) {
      display.drawString(63, n, "Volt " + String(batteryVoltage, 2));
      display.drawString(63, n+24, "T: " + String(t, 2));
      display.drawString(63, n+48, "H: " + String(h, 2));
      display.display();
      delay(10);
      display.clear();
  }




  

  Serial.println();
  Serial.println("lastTemperature" +String(lastTemperature) );
  // alarm if window open  
  if (lastTemperature - t > 4 && t < 22) { 
    ledcWriteTone(channel, 1700); 
    delay(120000);
  }

  Serial.println("batteryVoltage " +String(batteryVoltage) );
  while (true){//(batteryVoltage < 3.1 && batteryVoltage > 2.9 ) {
    ledcWriteTone(channel, 1700); 
    Serial.print("Low Voltage Alarm");
  }
  lastTemperature = t;
  delay(1000 *10);
  display.clear();
  display.display();

  // printPins();
   
  Serial.println(analogRead(batteryPin));
  if (batteryVoltage < 2.9 ) {
    adc_power_off();
  }
  // delay(1000 *60 *10);
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();

}

void printPins(){
    Serial.println((float)analogRead(13) /   4095*2*3.3*1.1 );
    Serial.println((float)analogRead(13) * (5.0 / 4095.0));
    Serial.println((float)analogRead(15) * (5.0 / 4095.0));    
    Serial.println((float)analogRead(3) * (5.0 / 4095.0));
    Serial.println((float)analogRead(A13) * (5.0 / 4095.0));
    Serial.println((float)analogRead(A15) * (5.0 / 4095.0));
    Serial.println((float)analogRead(A18) * (5.0 / 4095.0));
    Serial.println((float)analogRead(A19) * (5.0 / 4095.0));
    Serial.println((float)analogRead(A17) * (5.0 / 4095.0));    
    Serial.println((float)analogRead(A16) * (5.0 / 4095.0));
    Serial.println((float)analogRead(A15) * (5.0 / 4095.0));
    Serial.println((float)analogRead(A14) * (5.0 / 4095.0));
    
    Serial.println((float)analogRead(A10) * (5.0 / 4095.0));
    Serial.println((float)analogRead(A11) * (5.0 / 4095.0));
    Serial.println((float)analogRead(A12) * (5.0 / 4095.0));
    Serial.println((float)analogRead(A13) * (5.0 / 4095.0));
}
void loop() {
 

}
