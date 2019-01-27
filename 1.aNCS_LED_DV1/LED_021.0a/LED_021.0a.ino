#include <SoftwareSerial.h>
#include <Wire.h>
#define I2C_RS485 1
#define I2C_MAT_A 2
#define I2C_MAT_B 3
#define I2C_MAT_C 4
=======
#define I2C_MAT_D 5
#define I2C_MAT_E 6
#define I2C_MAT_F 7
#define I2C_MAT_G 8
#define I2C_MAT_H 9
SoftwareSerial BTSerial(2,3); // RX | TX
String string = "";
void setup()
{
  pinMode(13, OUTPUT);
  pinMode(9, OUTPUT);  // this pin will pull the HC-05 pin 34 (key pin) HIGH to switch module to AT mode
  digitalWrite(9, HIGH);
  Serial.begin(115200);
  Serial.println("Waiting for BT Comand:");
  BTSerial.begin(38400);
  BTSerial.write("HI!");
  Wire.begin();
}
void loop()
{
  while(BTSerial.available()){
     char character = BTSerial.read(); // Receive a single character from the software serial port
        string.concat(character); // Add the received character to the receive buffer
  }
  string.trim();
   if (string.startsWith("MAT")){
    Serial.print("Matrix "+string.substring(3,4));
    String which = string.substring(3,4);
    int i2cNum = 0;
    if (which == "A")i2cNum = I2C_MAT_A;
    else if (which == "B")i2cNum = I2C_MAT_B;
    else if (which == "C")i2cNum = I2C_MAT_C;
    else if (which == "D")i2cNum = I2C_MAT_D;
    else if (which == "E")i2cNum = I2C_MAT_E;
    else if (which == "F")i2cNum = I2C_MAT_F;
    else if (which == "G")i2cNum = I2C_MAT_G;
    else if (which == "H")i2cNum = I2C_MAT_H;
    /// Anything else leaves i2cNum as 0  
    Serial.println(which);
    Serial.println(string.substring(4));
    if (i2cNum != 0){ // Send to single matrix
      Wire.beginTransmission(i2cNum);
      sendMatrix();
      string = "";  // Delete string after sending to single matrix
    }
    else { // Send to all matrix
      Wire.beginTransmission(2);
      sendMatrix();
      Wire.beginTransmission(3);
      sendMatrix();
      Wire.beginTransmission(4);
      sendMatrix();
      Wire.beginTransmission(5);
      sendMatrix();
      Wire.beginTransmission(6);
      sendMatrix();
      Wire.beginTransmission(7);
      sendMatrix();
      Wire.beginTransmission(8);
      sendMatrix();
      Wire.beginTransmission(9);
      sendMatrix();
      string = ""; // Delete string after sending to multiple matrices      
    }
   }
   if (string.startsWith("LED"))
   {
    Serial.print ("LED ");
    Serial.println (string.substring(3));
    digitalWrite(13,HIGH);
    Wire.beginTransmission(1);
    String buff = string.substring(3);
    char buffer[32];
    buff.toCharArray(buffer, 32);
    Wire.write(buffer);
    Wire.endTransmission();
    delay(500);
    digitalWrite(13,LOW);
    string = "";
   }
   if (string.startsWith("ALL")){
      Wire.beginTransmission(1); // LEDS
      Serial.println("Sending to strips");
      String buff = string.substring(3);
      char buffer[32];
      buff.toCharArray(buffer, 32);
      Wire.write(buffer);
      Wire.endTransmission();
      Wire.beginTransmission(2); // MAT A
      Serial.println("Sending to MAT A");
      sendMatrix();
      Wire.beginTransmission(3); // MAT B
      Serial.println("Sending to MAT B");
      sendMatrix();
      Wire.beginTransmission(4); // MAT C
      Serial.println("Sending to MAT C");
      sendMatrix();
      Wire.beginTransmission(5); // MAT D
      Serial.println("Sending to MAT D");
      sendMatrix();
      Wire.beginTransmission(6); // MAT E
      Serial.println("Sending to MAT E");
      sendMatrix();
      Wire.beginTransmission(7); // MAT F
      Serial.println("Sending to MAT F");
      sendMatrix();
      Wire.beginTransmission(8); // MAT G
      Serial.println("Sending to MAT G");
      sendMatrix();
      Wire.beginTransmission(9); // MAT H
      Serial.println("Sending to MAT H");
      sendMatrix();
      string = ""; // Delete string after sending ALL
   }
   if (string != ""){
            Serial.println(string); //Output the message
           string =""; //clear the buffer/message
        }
// Keep reading from Arduino Serial Monitor and send to HC-05
  if (Serial.available())
    BTSerial.write(Serial.read());
}
void sendMatrix(){  //// Probably here that is issue ?? 
    String buff = string.substring(4);
    char buffer[buff.length()+1];
    buff.toCharArray(buffer, buff.length()+1);
    Wire.write(buffer);
    Wire.endTransmission();
    delay(500);
    digitalWrite(13,LOW);
      /// issue will be here as we only have a string object once, then we make it ""; 
      }
