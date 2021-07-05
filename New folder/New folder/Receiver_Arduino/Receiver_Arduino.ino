#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
int data; //Initialized variable to store recieved data

#define MAX_BRIGHTNESS 260
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET    -1 


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); 


void setup() {

   display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  
  //Serial Begin at 9600 Baud 
  Serial.begin(9600);

     display.clearDisplay();
  display.display();
}

void loop() {
  data = Serial.read(); //Read the serial data and store it
     display.clearDisplay();
     display.setTextSize(2);
     display.setTextColor(WHITE);
     display.setCursor(0, 2);
     display.print(data);
     display.display();
     delay(1000);
}
