#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
const char* ssid     = "Hariesh_Y5";
const char* password = "9918005113";
SoftwareSerial nodemcu(3,1);
WiFiServer server(80);
String header;

void setup() {
  
  Serial.begin(115200);
  Serial.print("\n Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();

}

void loop(){
   StaticJsonBuffer<1000> jsonBuffer;
   JsonObject& data = jsonBuffer.parseObject(nodemcu);
  if (data == JsonObject::invalid()) {
    Serial.println("Invalid Json Object");
    jsonBuffer.clear();
    return;
  }

   Serial.println("JSON Object Recieved");
   Serial.println("Spo2: ");
   float spo2 = data["spo2"];
   Serial.print(spo2);
   Serial.print("%  ");
   Serial.println("Temperature: ");
   float temp = data["temperature"];
   Serial.print(temp);
   Serial.println("----------------------------------");
   
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected())  {           // loop while the client's connected
        
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
              //Display the HTML web page
             client.println("<!DOCTYPE html><html>");
             client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
             client.println("<link rel=\"icon\" href=\"data:,\">");
              // Web Page Heading
             client.println("<body><h1>Welcome to CSP Web Server</h1>");
             client.println("<p>Spo2 " + String(spo2) + "</p>");
             client.println("<p>Temperature " + String(temp) + "</p>");
             client.println("</body></html>");
             client.println();
             break;
            
            
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
