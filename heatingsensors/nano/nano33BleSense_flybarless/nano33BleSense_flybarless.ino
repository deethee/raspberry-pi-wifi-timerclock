// accelaration
#include <Arduino_LSM9DS1.h>
//barometer
#include <Arduino_LPS22HB.h>
//gesture proximity
#include <Arduino_APDS9960.h>
// humidity
#include <Arduino_HTS221.h>
//This library supports all the Arduino boards that have the hardware enabled for BLE and Bluetooth 4.0 and above; these include Nano 33 BLE, Arduino NANO 33 IoT, Uno WiFi Rev 2, MKR WiFi 1010.
#include <ArduinoBLE.h>

//gyro
float acRX,acRY,acRZ,acX,acY,acZ;
int xAng,yAng,zAng;
int minVal=-2000;
int maxVal=2000;
//barometer
float h;
float pressure;
float groundZero;    
// proximity
int proximity;
// Temperature
float temperature = 0;
int oldTemperature = 0;  // last battery level reading from analog input


BLEService flybarlessInfoService("180F");
// BLE Battery Level Characteristic
BLEUnsignedCharCharacteristic flybarlessInfoChar("2A19",  // standard 16-bit characteristic UUID
BLERead | BLENotify); // remote clients will be able to get notifications if this characteristic changes

long previousMillis = 0;  // last time the battery level was checked, in ms
boolean isTemperatureEnabled = true;
boolean isGyroEnabled = false;
boolean isBarometerEnabled = true;
boolean isProximityEnabled = false;
boolean isBleEnabled = true;

void setup() {
    Serial.begin(9600);
    while (!Serial);
    Serial.println("Started");

    if (isGyroEnabled){
      if (!IMU.begin()) {
        Serial.println("Failed to initialize accelaration IMU!");
        while (1);
      }
    }
   if (isTemperatureEnabled){
      if (!HTS.begin()) {
        Serial.println("Failed to initialize humidity temperature sensor!");
        while (1);
      }
   }
  if (isBarometerEnabled){
    if (!BARO.begin()) {
      Serial.println("Failed to initialize pressure sensor!");
      while (1);
    }
    if (!APDS.begin()) {
      Serial.println("Failed to initialize proximity sensor!");
      while (1);
    }
  }
  if (isBleEnabled) {
    if (!BLE.begin()) {
      Serial.println("starting BLE failed!");
      while (1);
    }
  }
  BLE.setLocalName("FlybarlessInfo");
  BLE.setAdvertisedService(flybarlessInfoService); // add the service UUID
  flybarlessInfoService.addCharacteristic(flybarlessInfoChar); // add the battery level characteristic
  BLE.addService(flybarlessInfoService); // Add the battery service
  flybarlessInfoChar.writeValue(oldTemperature); // set initial value for this characteristic

  /* Start advertising BLE.  It will start continuously transmitting BLE
     advertising packets and will be visible to remote BLE central devices
     until it receives a new connection */

  // start advertising
  BLE.advertise();


  delay(3000);

  // averaging failure readings
  if (isGyroEnabled || isBarometerEnabled){
    for (int i =0; i < 300; i++) {
      if (isGyroEnabled){
        Serial.print("Accelerometer sample rate = ");
        Serial.print(IMU.accelerationSampleRate());
        Serial.println(" Hz");
        Serial.println();
        Serial.println("Acceleration in G's");
        
        Serial.print("Gyroscope sample rate = ");
        Serial.print(IMU.gyroscopeSampleRate());
        Serial.println(" Hz");
        Serial.println();
        Serial.println("Acceleration in G's");
      }
       if (isBarometerEnabled){
          float pressure = BARO.readPressure();
          pressure = pressure + pressure;
       }
       delay(10);
    }
    if (isGyroEnabled){      
    }
    if (isBarometerEnabled){
          groundZero = groundZero/300;
          Serial.print("Pressure initial= ");
          Serial.print(pressure);
          Serial.println(" kPa");
    }

  }


    

}
void loop() {

 

if (isGyroEnabled) {
  if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(acX, acY, acZ);
      Serial.print("acX = ");
      Serial.print(acX);
      Serial.print("acY =");
      Serial.print(acY);
      Serial.print("acZ :");
      Serial.print(acZ);
      Serial.println(" )");
  }
  
  if (IMU.gyroscopeAvailable()) {
      IMU.readGyroscope(acRX, acRY, acRZ);
      Serial.print("Gyroscope rotation speed: ( x: ");
      Serial.print(acRX);
      Serial.print(" / y:");
      Serial.print(acRY);
      Serial.print(" / z:");
      Serial.print(acRZ);
      Serial.println(" )");  
  
      xAng = map(acRX,minVal,maxVal,-90,90);
      yAng = map(acRY,minVal,maxVal,-90,90);
      zAng = map(acRZ,minVal,maxVal,-90,90);
      double x;
      double y;
      double z;
      
      x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);
      y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);
      z= RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);
  
     Serial.print("AngleX= ");
     Serial.println(x);

     Serial.print("AngleY= ");
     Serial.println(y);

     Serial.print("AngleZ= ");
     Serial.println(z);
     Serial.println("-----------------------------------------");
  }
}
   if (isBarometerEnabled){
        float pressure = BARO.readPressure();
        h = (pressure - groundZero) / 1;
        Serial.print("Höhe = ");
        Serial.println(h);
   
  }  
   if (isProximityEnabled){
        if (APDS.proximityAvailable()) {
          proximity = APDS.readProximity();
          Serial.print("Proximity = ");
          Serial.println(proximity);
        }
   }      
  if (isTemperatureEnabled){
    temperature = HTS.readTemperature()-5;
    Serial.print("Temperatur = ");
    Serial.println(temperature);
    
    float humidity = HTS.readHumidity();
    Serial.print("Feuchtigkeit = ");
    Serial.println(humidity);
  }
  if (isBleEnabled) {
    BLEDevice central = BLE.central();
    if (central) {
    Serial.print("Connected to central: ");
    // print the central's BT address:
    Serial.println(central.address());
    // turn on the LED to indicate the connection:
    digitalWrite(LED_BUILTIN, HIGH);

    // check the battery level every 200ms
    // while the central is connected:
    while (central.connected()) {
      long currentMillis = millis();
      // if 200ms have passed, check the battery level:
      if (currentMillis - previousMillis >= 200) {
        previousMillis = currentMillis;
        if (temperature != oldTemperature) {  
              Serial.print("temperature % is now: "); // print it
              Serial.println(temperature);
              flybarlessInfoChar.writeValue(temperature); 
              oldTemperature = temperature;
        }
        
        }
    }
    // when the central disconnects, turn off the LED:
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
  }

  delay(1000);
}
