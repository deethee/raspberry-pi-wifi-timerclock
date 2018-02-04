// Replace with your network credentials
  const char* ssid     = "xxx";
  const char* password = "xxx";
  const char* iftttToken_TestInternetConnection =  "xxx";
  const char* iftttToken_sendMailMissing = "xxx";


// pins
  const int DHTPin = 15; // Temperature sensor
  int motionDetectionSignalPin = 12;
  int lightDetectionSignalPin = 35; // on esp32 above 32 
  int ledSignalPIN = 0; 
  int relaySignalPin = 4;


// Wifi
  #include <HTTPClient.h>
  #include <WiFi.h>


// Tempearture and humidity measure
  #include "DHT.h"
  #define DHTTYPE DHT11   // DHT sensor type 11
  // Initialize DHT sensor.
  DHT dht(DHTPin, DHTTYPE);  
  // Temporary variables
  static char celsiusTemp[7];
  static char humidityTemp[7];


// Wemos xring
  #include <Adafruit_NeoPixel.h> // https://github.com/adafruit/Adafruit_NeoPixel
  int countLeds = 12;
  // Parameter 1 = number of pixels in strip
  // Parameter 2 = pin number (most are valid)
  // Parameter 3 = pixel type flags, add together as needed:
  //   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
  //   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
  //   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
  //   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
  Adafruit_NeoPixel strip = Adafruit_NeoPixel(countLeds, ledSignalPIN, NEO_GRB + NEO_KHZ800);
  int randomColorCodeR = random(255); 
  int randomColorCodeG = random(255);
  int randomColorCodeB = random(255);

// test network wifi 
  int internetConnectionTesterCounter = 0;
  boolean failedConnection = false;

// motion detection
  int motionDetectionState = LOW;  // we start, assuming no motion detected
  int motionDetectionValue = 0;
  int awayCounter = 0;

// light detection on 3.3v!
  int lightDetectionValue = 0;


void setup() {

  // initialize the DHT sensor
  dht.begin();  
  
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while(!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // init xring
 strip.begin();
 for(int i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(random(0, 255), random(0, 255), random(0, 255)));
      strip.setBrightness(255);
      strip.show();
      delay(random(10, 300));
 }  
 delay(1000);
 for(int i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(50, 0, 0));
      strip.setBrightness(5);
      strip.show();
      delay(10);
  }
  delay(2000);
  ledsOff();
  // end init xring


  // init Wifi
  // If power plug wait for router to startup
  Serial.print("wait 10 min for router to startup");
  delay(1000 * 60 * 10);
  wifiConnect();  
  // END iInit Wifi

  // motion & light detection
   pinMode(motionDetectionSignalPin, INPUT);        // sets the digital pin 12 as input
   pinMode(lightDetectionSignalPin, INPUT);

  // relay
   pinMode(relaySignalPin, OUTPUT);
   digitalWrite(relaySignalPin, LOW);
}

void loop() {
  wifiConnect();
  if (awayCounter>0){
    //Serial.println("AwayCounter:" + String(awayCounter));
  }
  motionDetectionValue = digitalRead(motionDetectionSignalPin);  // read input value
  if (motionDetectionValue == HIGH) {            // check if the input is HIGH
    
    if (motionDetectionState == LOW) {
      // we have just turned on
      Serial.println("Motion detected!");
      // We only want to print on the output change, not state
   
      ledsRandomColor();  // turn LED ON
      awayCounter = 0;  
    }   

  } else {
    if (awayCounter>30){
        ledsOff(); 
        randomColorCodeR = random(255);
        randomColorCodeG = random(255);
        randomColorCodeB = random(255); 
    }
    if (motionDetectionState == HIGH){      
      // we have just turned of
      Serial.println("Motion ended!");
      // We only want to print on the output change, not state
      motionDetectionState = LOW;         
    }
    awayCounter++;
   if (awayCounter % (60 * 60 * 14) == 0){
      sendMailMissing(awayCounter / (60 * 60 * 14) );
    }


    
  }

    internetConnectionTesterCounter++;

    if (internetConnectionTesterCounter % (24 * 60 * 60   ) == 0){ // Test internet connection daily
      testingInternetConnection();
      internetConnectionTesterCounter=0;
    } 
  
    delay(1000);  
  }


void wifiConnect(){  
  if(WiFi.status() != WL_CONNECTED) {
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
}

void ledsRandomColor() {

  lightDetectionValue = analogRead(lightDetectionSignalPin); 
  //Serial.println("lightDetectionValue:" + String(lightDetectionValue));
  
  if(lightDetectionValue>0){ 
   //Serial.println("sitching leds on it is dark enough");
   motionDetectionState = HIGH;
     if(!failedConnection){
     for(int i=0; i<strip.numPixels(); i++) {
          strip.setPixelColor(i, strip.Color(randomColorCodeR, randomColorCodeG, randomColorCodeB));
          strip.setBrightness(255);
          strip.show();
          delay(0);
     } 
   }
  }else{
    Serial.println("not switching leds on due to daylight:");
     motionDetectionState = LOW;
  }
    // blink 100 times
   if(failedConnection){
    for(int i=0; i<10; i++){
     for(int i=0; i<strip.numPixels(); i++) {
          strip.setPixelColor(i, strip.Color(0, 255, 0));
          strip.setBrightness(255);
          strip.show();
          delay(10);
     }
     for(int i=0; i<strip.numPixels(); i++) {
          strip.setPixelColor(i, strip.Color(0, 0, 0));
          strip.setBrightness(0);
          strip.show();
          delay(10);
     }
   }
   }  

  
}

void ledsOff() {
 for(int i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
      strip.setBrightness(0);
      strip.show();
      delay(0);
 } 
}

void sendTemperature() {
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

           // Check if any reads failed and exit early (to try again).
            if (isnan(h) || isnan(t) ) {
              Serial.println("Failed to read from DHT sensor!");
              // xring turns red
              strcpy(celsiusTemp,"Failed");
              strcpy(humidityTemp, "Failed");       
              for(int i=0; i<strip.numPixels(); i++) {
                    strip.setPixelColor(i, strip.Color(0, 255, 0));
                    strip.setBrightness(3);
                    strip.show();
                    delay(0);
              }
              // END xring turns red
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
}


void sendMailMissing(int times){
        Serial.println("-----------------------------------------send mail");;

        HTTPClient http;   
        http.begin("http://maker.ifttt.com/trigger/" + iftttToken_sendMailMissing);
              
        http.addHeader("Content-Type", "application/json");
        int httpCode = http.POST("{\"value1\":"+String(8 * times)+"}");
        http.writeToStream(&Serial);
        http.end();       

        if (httpCode == 200 || httpCode == 201){
            failedConnection = false;
            Serial.println("httpCode" + String(httpCode));
        } else {
            failedConnection = true;
            Serial.println("Fail httpCode" + String(httpCode));
            restartRouter();
        }   
}





void testingInternetConnection() {
    Serial.println("-----------------------------------------test internet Connection");

              HTTPClient http;   
              http.begin("http://maker.ifttt.com/trigger/" +iftttToken_TestInternetConnection );
              
              http.addHeader("Content-Type", "application/json");
              int httpCode = http.POST("{\"value1\":"+String(8)+"}");
              http.writeToStream(&Serial);
              http.end();  

              if (httpCode == 200 || httpCode == 201){
                  failedConnection = false;
                  Serial.println("Successfull httpCode:" + String(httpCode));
              } else {
                  failedConnection = true;
                  Serial.println("Fail httpCode:" + String(httpCode));
                  restartRouter();
              }
               
}


void restartRouter() {

    // switch off for 10 min
    Serial.println("restart router");
    digitalWrite(relaySignalPin, HIGH);
    delay(1000*60);
    digitalWrite(relaySignalPin, LOW);

    // test connection again if ok than continue else 
    HTTPClient http;   
    http.begin("http://maker.ifttt.com/trigger/" +iftttToken_TestInternetConnection );
    
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST("{\"value1\":"+String(8)+"}");
    http.writeToStream(&Serial);
    http.end();  

    if (httpCode == 200 || httpCode == 201){
        failedConnection = false;
        Serial.println("Successful httpCode:" + String(httpCode));
    } else {
        failedConnection = false;
        Serial.println("Failed httpCode:" + String(httpCode));
    }
    
  }


