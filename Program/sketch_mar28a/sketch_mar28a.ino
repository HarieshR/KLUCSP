// Arduino-Nodemcu comm
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

//Initialise Arduino to NodeMCU (5=Rx & 6=Tx)
SoftwareSerial nodemcu(10, 11);

void setup() {
 
  nodemcu.begin(115200);

}

void loop()
   {
      StaticJsonBuffer<1000> jsonBuffer;
      JsonObject& data = jsonBuffer.createObject();
      data["spo2"] = 45;
      data["temperature"] = 44;
      data.printTo(nodemcu);
      jsonBuffer.clear();
    }
 
