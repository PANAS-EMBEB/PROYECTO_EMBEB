#include <Wire.h> 

int LED = 13;
int x = 5;

void setup() {
  pinMode (LED, OUTPUT);          // Define the LED pin as Output 

  Wire.begin(9);                  // Start the I2C Bus with address 9 in decimal (= 0x09 in hexadecimal) 

  Wire.onReceive(receiveEvent);   // Attach a function to trigger when something is received 
}

void receiveEvent() {
  x = Wire.read();                // read one character from the I2C 
}

void loop() {

  //If value received is a multiple of 2  turn on the led 
  if (x % 2 == 0) {
    digitalWrite(LED, HIGH);
  }

  //Else turn off the led 
  else {
    digitalWrite(LED, LOW);
  }
} 
