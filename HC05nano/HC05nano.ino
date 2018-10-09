#include <SoftwareSerial.h>
#include <Wire.h>

SoftwareSerial BTSerial(2, 3); // RX | TX
String string = "";
String string2 = "LED";
void setup()
{
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  Serial.println("Enter a command");
  BTSerial.begin(38400);
  Wire.begin();
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
           string =""; //clear the buffer/message
        }

// Keep reading from Arduino Serial Monitor and send to HC-05
  if (Serial.available())
    BTSerial.write(Serial.read());
}
