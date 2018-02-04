#include <WiFi.h>

const char* ssid     = "xxx";
const char* password = "xxx";
WiFiServer server(80);

int signalPin = 22;

void setup() {
    Serial.begin(115200);
    pinMode(5, OUTPUT);      // set the LED pin mode

    delay(10);

    // We start by connecting to a WiFi network
    wifiConnect();
    
    server.begin();
    pinMode(signalPin, OUTPUT);
    digitalWrite(signalPin, LOW);
}



int value = 0;

void loop(){

 wifiConnect();
  
 WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print("<a href=\"/toggle\">Garagentor betaetien</a><br>");
            // the content of the HTTP response follows the header:
            //client.print("Garagentor betaetigt");
  

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("/toggle")) {
                  toggleDoor();
        }

      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
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

void toggleDoor(){
  digitalWrite(signalPin, HIGH);
  delay(200);
  digitalWrite(signalPin, LOW);
}

