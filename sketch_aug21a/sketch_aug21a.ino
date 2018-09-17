#include <SoftwareSerial.h>
#include <Wire.h>

SoftwareSerial BTSerial(8, 9); // RX | TX
String string = "";
String string2 = "LED";
void setup()
{
  pinMode(13, OUTPUT);
  pinMode(9, OUTPUT);  // this pin will pull the HC-05 pin 34 (key pin) HIGH to switch module to AT mode
  digitalWrite(9, HIGH);
  Serial.begin(9600);
  Serial.println("Enter AT commands:");
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
   if (string.startsWith("POL"))
   {
    Wire.requestFrom(8,32);
    String str = "{";
    while (Wire.available()) { // slave may send less than requested
      char c = Wire.read(); // receive a byte as character
      str.concat(c);
    }
    str.trim();
    str += "}";
    Serial.println(str);
    char buffr[64];
    str.toCharArray(buffr,64);
    BTSerial.write(buffr);
   }
   if (string.startsWith("LED"))
   {
    Serial.print ("LED shown ");
    Serial.println (string.substring(3));
    digitalWrite(13,HIGH);
    Wire.beginTransmission(8);
    String buff = string.substring(3);
    char buffer[32];
    buff.toCharArray(buffer, 32);
    Wire.write(buffer);
    Wire.endTransmission();
    delay(500);
    digitalWrite(13,LOW);
    string = "";
   }
   if (string != ""){
            Serial.println(string); //Output the message
           string =""; //clear the buffer/message
        }

// Keep reading from Arduino Serial Monitor and send to HC-05
  if (Serial.available())
    BTSerial.write(Serial.read());
}
