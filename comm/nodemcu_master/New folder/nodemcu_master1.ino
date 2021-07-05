//ThatsEngineering
//Sending Data from Arduino to NodeMCU Via Serial Communication
//NodeMCU code

//Include Lib for Arduino to Nodemcu
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266_Lib.h>
//D6 = Rx & D5 = Tx
SoftwareSerial Serial3(D6, D5);
WiFiServer server(80);

String header;
String LED1State = "off";
const int LED1 = D0;

void setup() {
  // Initialize Serial port
  Serial3.begin(115200);

  Serial3.println();

  WiFi.begin("Hariesh_Y5", "9918005113");

  Serial3.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial3.print(".");
  }
  Serial3.println();

  Serial3.print("Connected, IP address: ");
  Serial3.println(WiFi.localIP());

  server.begin();


  nodemcu.begin(9600);
  while (!Serial3) continue;

}

void loop() {

  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& data = jsonBuffer.parseObject(nodemcu);

  if (data == JsonObject::invalid()) {
    //Serial.println("Invalid Json Object");
    jsonBuffer.clear();
    return;
  }

  Serial3.println("JSON Object Recieved");
  Serial3.print("Recieved Spo2:  ");
  float spo2 = data["spo2"];
  Serial3.println(spo2);
  Serial3.print("Recieved Temperature:  ");
  float temp = data["temperature"];
  Serial3.println(temp);
  Serial3.println("-----------------------------------------");

  WiFiClient client = server.available();
  if (client) {                             // If a new client connects,
    Serial3.println("new client connected");
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected())
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();
        Serial3.write(c);
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
  Serial3.println("Client disconnected");

}
