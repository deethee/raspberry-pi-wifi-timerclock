
// Oled wemos will not compile or upload with sensor connected !!!!!!!

//pins
int motionDetectionSignalPin = 34; // Analog 5V , A0, GND  
const int DHTPin = 2; // digital 3.3v, 2, GND
uint8_t ledPin = 16; // Onboard LED reference
uint8_t oledPin1 = 4; // Onboard LED reference
uint8_t oledPin2 = 5; // Onboard LED reference
int batteryPin = 13;// is the gpio pin not analog
int buzzerPin = 15;

// Temperature
#include "DHT.h"
#define DHTTYPE DHT11 
DHT dht(DHTPin, DHTTYPE);
float temperature;
float humidity;


// Wifi
#include <WiFi.h>
#include "arduino_secrets.h"
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;
const char* url = "http://192.168.0.37";
const int port = 4200;
#include <HTTPClient.h>

// buzzer
int freq = 1700;

int resolution = 8;
boolean isAlarm = false;

// oled
#include "SSD1306.h"
SSD1306 display(0x3c, 5, 4); // instance for the OLED. Addr, SDA, SCL

// deep sleep
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  600        /* Time ESP32 will go to sleep (in seconds) */
RTC_DATA_ATTR float lastTemperature;
#include "driver/adc.h"

//Battery Voltage
float batteryVoltage; // not working esp32 measures up to 3.3v needs voltage devider


void setupSensors() {
  Serial.begin(9600);
  digitalWrite(ledPin, HIGH);
  dht.begin();
  pinMode(batteryPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  pinMode(motionDetectionSignalPin, INPUT); 
  
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
  ledcSetup(buzzerPin, freq, resolution);
  ledcAttachPin(0, buzzerPin);

  // oled
  display.init(); // initialise the OLED
  //display.flipScreenVertically(); // does what is says
  display.setFont(ArialMT_Plain_16); // does what is says
  // Set the origin of text to top left
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);

  // wait for sensors initialize
  delay(5000);
  digitalWrite(ledPin, LOW);
  Serial.println("Setup Sensors Complete");
}


void setup() {
  
  setupSensors(); 

  printOled();
 
  Serial.println("batteryVoltage " +String(batteryVoltage) );
  if (batteryVoltage < 3.1 && batteryVoltage > 2.9 ) {
    Serial.print("Low Voltage Alarm");    
    display.drawString(63, 14, "Akku Laden" );
     display.drawString(63, 14, "Volt->" + String(batteryVoltage, 2));
    isAlarm = true;
  }
  if (batteryVoltage <= 2.9 ) {
    Serial.print("Shutdown Voltage");
    adc_power_off();
  }

  Serial.println();
  Serial.println("lastTemperature" + String(lastTemperature) ); 
  if (lastTemperature - temperature > 4 && temperature < 22) {
    Serial.print("window open alarm");
    isAlarm = true;
    display.drawString(63, 14, "Fenster offen" );
    display.drawString(63, 28, "Differenz T:" + String(temperature-lastTemperature, 2));
    display.display();
  }


  if (!isAlarm){ // goto deep sleep 
     deepSleep();
  } else {
    alarmLoop();
  }

}

void deepSleep(){
    Serial.println("Initiating deepsleep");

    lastTemperature = temperature;
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    esp_deep_sleep_start();
}
void printOled(){

  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 3.3V):
  batteryVoltage = analogRead(batteryPin) * (3.3 / 4095.0);
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  
  if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT");
    } else {
      Serial.println("Temperature = "+String(temperature));
      Serial.println("Humidity = "+String(humidity));
      HTTPClient http;   
      http.begin("http://192.168.0.37:8080/api/sensorvalue/");  
      http.addHeader("Content-Type", "application/json");
      http.POST("{\"temperature\":"+String(temperature)+",\"humidity\":"+String(humidity)+"}");
      http.writeToStream(&Serial);      
      http.end();    
  }
  
  // Upwards scroll OLED text
  for(int8_t n = 112; n > 0; n--) {
  //int8_t n = 0;
      display.drawString(63, n+16, "Volt " + String(batteryVoltage, 2));
      display.drawString(63, n+34, "Temperatur " + String(temperature, 2));
      display.drawString(63, n+52, "Feuchtigkeit  " + String(humidity, 2));
      display.display();
      delay(10);
      display.clear();
  }

  delay(1000 *10);
  display.clear();
  display.display();
  Serial.println("finished oled");
}
void alarmBuzz(){
 //Serial.println("startbuzz");
 ledcWriteTone(buzzerPin, 2000);
  /*
  for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle=dutyCycle+10){  
    Serial.println(dutyCycle);  
    ledcWrite(buzzerPin, dutyCycle);
  }
  */
  ledcWrite(buzzerPin, 125);
  
  for (int freq = 255; freq < 10000; freq = freq + 250){  
     ledcWriteTone(buzzerPin, freq);
     delay(100);
  }
  //Serial.println("endbuzzbuzz");
  alarmLoop();
}

void alarmLoop(){
       int motionDetectionValue = analogRead(motionDetectionSignalPin) ; 
       Serial.print("motion: "); Serial.println(motionDetectionValue);
       
      if (motionDetectionValue > 100){ 
        isAlarm = false;
        Serial.print("motion detected going deepsleep: "); Serial.println(motionDetectionValue);
        deepSleep();
      } else{
        alarmBuzz();    
      }
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
