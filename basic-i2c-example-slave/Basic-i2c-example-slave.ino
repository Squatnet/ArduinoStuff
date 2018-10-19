#include <Wire.h>
String string = "";
void setup() {
  pinMode(13, OUTPUT);
  Wire.begin(2);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600); 
  digitalWrite(13,LOW);// start serial for output
}

void loop() {
  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  while (Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    string.concat(c); 
  }
  string.concat(Wire.read());
  Serial.println(string);
  string = "";// receive byte as an integer  
  }// print the integer
