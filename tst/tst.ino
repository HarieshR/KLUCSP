#include <Keypad.h>
#include <ctype.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

//Initialise Arduino to NodeMCU (5=Rx & 6=Tx)
SoftwareSerial nodemcu(0,1);
int spo2=98;
int temp=94;

void setup() {
    nodemcu.begin(115200);
}

void loop()

    {
      StaticJsonBuffer<1000> jsonBuffer;
      JsonObject& data = jsonBuffer.createObject();
      data["spo2"] = spo2;
      data["temperature"] = temp;
      data.printTo(nodemcu);
      jsonBuffer.clear();
    }
