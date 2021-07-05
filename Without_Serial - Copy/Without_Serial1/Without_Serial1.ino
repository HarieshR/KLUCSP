#include <Keypad.h>
#include <ctype.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

// Arduino-Nodemcu comm
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

//Initialise Arduino to NodeMCU (5=Rx & 6=Tx)
SoftwareSerial nodemcu(0,1);
// Arduino-Nodemcu comm

MAX30105 particleSensor;

#define MAX_BRIGHTNESS 255
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

unsigned long loopCount;
unsigned long startTime;
String msg;

// Spo2

#if defined(_AVR_ATmega328P) || defined(AVR_ATmega168_)

uint16_t irBuffer[100];
uint16_t redBuffer[100];
#else
uint32_t irBuffer[100];
uint32_t redBuffer[100];
#endif

int32_t bufferLength;
int32_t spo2;
int8_t validSPO2;
int32_t heartRate;
int8_t validHeartRate;

byte pulseLED = 11;
byte readLED = 13;



void setup() {
  //  Serial.begin(115200);

  // Arduino-Nodemcu comm
  nodemcu.begin(115200);
  // Arduino-Nodemcu comm

  mlx.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);


  pinMode(pulseLED, OUTPUT);
  pinMode(readLED, OUTPUT);

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
  {
    //    Serial.println(F("MAX30105 was not found. Please check power."));
    while (1);
  }

  //  Serial.println(F("Attach sensor to finger with rubber band. Press any key to start conversion"));
  //  while (Serial.available() == 0) ; //wait until user presses a key
  //  Serial.read();

  byte ledBrightness = 180;
  byte sampleAverage = 4;
  byte ledMode = 2;
  byte sampleRate = 100;
  int pulseWidth = 411;
  int adcRange = 4096;

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);

  display.clearDisplay();
  display.display();
  nodemcu.begin(115200);
}

void loop()

{
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 2);
  display.print("    CSP\n BATCH 39\n WELCOMES \n   YOU!");
  display.setTextSize(1);
  display.setCursor(0, 55);
  display.display();
  delay(5000);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 2);
  display.println("Please \nenter your\nReg Number");

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 55);
  display.println("press '*' to restart");

  display.display();
  char value[11] = ""; // the number accumulator
  int count = 0;
  unsigned long keyvalue;
  unsigned long isnum;
  //  unsigned long s=0;
  //  Serial.println("Enter Reg no, press # to end ");
  //  Serial.print("You have typed: ");
  do
  {

    keyvalue = keypad.getKey();
    isnum = (keyvalue >= '0' && keyvalue <= '9');
    if (isnum)
    {
      //      Serial.print(keyvalue - '0');
      //         s= s*10+(keyvalue - '0');
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0, 5);
      display.println("Reg num:");

      //  display.setTextSize(2);
      //  display.setCursor(0, 40);
      //  display.println(s);
      // display.display();

      value[count] = keyvalue;
      count = count + 1;
      display.setTextSize(2);
      display.setCursor(0, 40);
      display.println(value);
      display.display();
    }

  } while (isnum || !keyvalue);
  {
    if (keyvalue == '*')
      return false;
    else

      //      Serial.println(" ");
      //    Serial.println("Returning from funtion: ");
      //    Serial.println(value);

      display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 2);
    display.println("Hai,");

    display.setTextSize(1);
    display.setCursor(28, 2);
    display.println(value);
    display.display();
    // delay(400);
  }
  //getKeypadInteger



  // Spo2

  bufferLength = 100;

  //read the first 100 samples, and determine the signal range
  for (byte i = 0 ; i < bufferLength ; i++)
  {
    while (particleSensor.available() == false)
      particleSensor.check();

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample();

    //    Serial.print(F("red="));
    //    Serial.print(redBuffer[i], DEC);
    //    Serial.print(F(", ir="));
    //    Serial.println(irBuffer[i], DEC);
  }


  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);


  while (1)
  {

    for (byte i = 25; i < 100; i++)
    {
      redBuffer[i - 25] = redBuffer[i];
      irBuffer[i - 25] = irBuffer[i];
    }


    for (byte i = 75; i < 100; i++)
    {
      while (particleSensor.available() == false)
        particleSensor.check();
      digitalWrite(readLED, !digitalRead(readLED));

      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample();

      //send samples and calculation result to terminal program through UART
      //      Serial.print(F("red="));
      //      Serial.print(redBuffer[i], DEC);
      //      Serial.print(F(", ir="));
      //      Serial.print(irBuffer[i], DEC);

      //      Serial.print(F(", HR="));
      //      Serial.print(heartRate, DEC);
      //
      //      Serial.print(F(", HRvalid="));
      //      Serial.print(validHeartRate, DEC);
      //
      //      Serial.print(F(", SPO2="));
      //      Serial.print(spo2, DEC);
      //
      //      Serial.print(F(", SPO2Valid="));
      //      Serial.print(validSPO2, DEC);
      //
      //      Serial.print(F(", Temperature"));
      //      Serial.println(mlx.readObjectTempF(), 1);
      //try

      // Temp display

      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 2);
      display.println("Hai,");

      display.setTextSize(1);
      display.setCursor(28, 2);
      display.println(value);

      if (heartRate > 60 && heartRate < 110)
      {
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 16);
        display.println("Pulse");

        //  display.setTextSize(1);
        //  display.setCursor(50, 16);
        //  display.setTextColor(WHITE, BLACK);
        //  display.println("       ");

        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(50, 16);
        display.println(heartRate, DEC);

        display.setCursor(110, 16);
        display.println("BPM");
      }
      else
      {
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0, 16);
        display.println("place finger firmly");

      }
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 35);
      display.println("Temp");

      display.setTextSize(2);
      display.setCursor(50, 30);
      display.println(mlx.readObjectTempF(), 1);

      display.setCursor(110, 30);
      display.println("F");


      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 55);
      display.println("spo2");

      display.setTextSize(1);
      display.setCursor(110, 50);
      display.println("%");
      if (spo2 > 80)
      {
        display.setTextSize(1);
        display.setCursor(50, 55);
        display.setTextColor(WHITE, BLACK);
        display.println("       ");

        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(50, 50);
        display.println(spo2, DEC);
      }
      else
      { display.setTextSize(1);
        display.setCursor(50, 55);
        display.println("Loading");
      }


      display.display();
      delay(500);

    }

    // Arduino-Nodemcu comm
//    if (validSPO2)
    {
      StaticJsonBuffer<1000> jsonBuffer;
      JsonObject& data = jsonBuffer.createObject();
      data["spo2"] = spo2;
      data["temperature"] = mlx.readObjectTempF();
      data.printTo(nodemcu);
      jsonBuffer.clear();
    } 
    // Arduino-Nodemcu comm

    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  }
  display.display();
  //  delay(500);
 
}
