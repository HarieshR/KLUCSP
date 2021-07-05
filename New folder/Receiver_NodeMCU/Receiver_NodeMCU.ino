#include<SoftwareSerial.h> 
//Started SoftwareSerial at RX and TX 
SoftwareSerial s(3,1);
char data; //Initialized variable to store recieved data

void setup() {
Serial.begin(9600);
 s.begin(9600);
   }

void loop() {
  data = Serial.read(); //Read the serial data and store it
Serial.print(data);  
                     
   }
