#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
     
SoftwareSerial nodemcu(16, 5);
WiFiServer server(80);

String header;
String LED1State = "off";
const int LED1 = 16;

void setup() {
  // Initialize Serial port
  Serial.begin(115200);

  Serial.println();

  WiFi.begin("Hariesh_Y5", "9918005113");

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();


  nodemcu.begin(115200);
//  while (!Serial) continue;

}

void loop() {

  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& data = jsonBuffer.parseObject(nodemcu);

//  if (data == JsonObject::invalid()) {
//    Serial.println("Invalid Json Object");
//    jsonBuffer.clear();
//    return;
//  }

  Serial.println("JSON Object Recieved");
  Serial.print("Recieved Spo2:  ");
  float spo2 = data["spo2"];
  Serial.println(spo2);
  Serial.print("Recieved Temperature:  ");
  float temp = data["temperature"];
  Serial.println(temp);
  Serial.println("-----------------------------------------");

  WiFiClient client = server.available();
  if (client) {                             // If a new client connects,
    Serial.println("new client connected");
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected())
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // Web Page Heading
            client.println("<body><h1>Welcome to ESP12e Web Server</h1>");
            client.println("<p>Spo2 " + String(spo2) + "</p>");
            client.println("<p>Temperature " + String(temp) + "</p>");
            client.println("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
  }
  header = "";
  client.stop();
  Serial.println("Client disconnected");

}
