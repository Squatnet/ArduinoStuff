#include <Wire.h>
void setup() {
  pinMode(13, OUTPUT);
  Wire.begin(8);                // join i2c bus with address #8
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
  int x = Wire.read();    // receive byte as an integer
  if (x == 1){
    digitalWrite(13,HIGH);
    delay(2000);
    digitalWrite(13,LOW);
  }
  Serial.println(x);   
  }// print the integer
}
