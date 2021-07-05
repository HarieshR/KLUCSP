#include<SoftwareSerial.h> 
//Started SoftwareSerial at RX and TX 
SoftwareSerial s(0,1);

void setup() {
  //Serial S Begin at 9600 Baud
  s.begin(9600);
}

void loop() {
  //Write '123' to Serial
  s.write("Hariesh\n");
  delay(100);
 
}
