
#include <Keypad.h>

const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 3; //three columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte pin_rows[ROW_NUM] = {2, 3, 4, 5};   //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {6, 7, 8}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

String inputString;
long inputInt;

void setup() {
  Serial.begin(115200);
  inputString.reserve(10); // maximum number of digit for a number is 10, change if needed
}

void loop() {
  char key = keypad.getKey();
  
  if (key){
    if (key >= '0' && key <= '9') {     // only act on numeric keys
      Serial.print(key - '0');
      inputString += key;               // append new character to input string
Serial.println(inputInt);
    } else if (key == '#') {
      if (inputString.length() > 0) {
        inputInt = inputString.toInt(); // YOU GOT AN INTEGER NUMBER
        Serial.println(inputInt);
        inputString = "";               // clear input
        // DO YOUR WORK HERE

 
      }
    } else if (key == '*') {
      inputString = "";                 // clear input
    }
  }
}
