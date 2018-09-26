#include <SoftwareSerial.h>
#include <Wire.h>

SoftwareSerial BTSerial(3, 2); // RX | TX
String string = "";
void setup()
{
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  Serial.println("Enter a command");
  BTSerial.begin(38400);
   Wire.begin(8);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
}
void requestEvent() {
  if (string != ""){
    char bufer[8];
    Serial.println("REQ "+string);
    string.toCharArray(bufer, 8);
    Wire.write(bufer);
    string = "";
  }
  else{
    Serial.println("Nothing Happened since last request");
  }
}
void receiveEvent(int howMany) {
  if (string != ""){
    char buff[8];
    Serial.println("REC "+string);
    string.toCharArray(buff, 8);
    Wire.write(buff);
    string = "";
  }
  else{
    Serial.println("Nothing Happened since last receive");
  }
}
void loop()
{
  while(BTSerial.available()){
     char character = BTSerial.read(); // Receive a single character from the software serial port
        string.concat(character); // Add the received character to the receive buffer
  }
  string.trim();
  if (string != ""){
            Serial.println(string); //Output the message
          // string =""; //clear the buffer/message
        }

// Keep reading from Arduino Serial Monitor and send to HC-05
  if (Serial.available())
    BTSerial.write(Serial.read());
}
