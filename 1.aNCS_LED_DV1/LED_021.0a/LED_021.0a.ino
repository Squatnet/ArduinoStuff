#include <SoftwareSerial.h>
#include <Wire.h>
#define I2C_RS485 1
#define I2C_MAT_A 2
#define I2C_MAT_B 3
#define I2C_MAT_C 4
SoftwareSerial BTSerial(2,3); // RX | TX
String string = "";
void setup()
{
  pinMode(13, OUTPUT);
  pinMode(9, OUTPUT);  // this pin will pull the HC-05 pin 34 (key pin) HIGH to switch module to AT mode
  digitalWrite(9, HIGH);
  Serial.begin(9600);
  Serial.println("Waiting for BT Comand:");
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
   if (string.startsWith("MAT")){
    Serial.print("Matrix "+string.substring(3,4));
    String which = string.substring(3,4);
    int i2cNum = 0;
    if (which == "A")i2cNum = I2C_MAT_A;
    else if (which == "B")i2cNum = I2C_MAT_B;
    else if (which == "C")i2cNum = I2C_MAT_C;    
    Serial.println(which);
    Serial.println(string.substring(4));
    if (i2cNum != 0){
      Wire.beginTransmission(i2cNum);
      sendMatrix();
    }
    else {
      Wire.beginTransmission(2);
      sendMatrix();
      Wire.beginTransmission(3);
      sendMatrix();
      Wire.beginTransmission(4);
      sendMatrix();
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
      Wire.beginTransmission(1);
      sendMatrix();
      Wire.beginTransmission(2);
      sendMatrix();
      Wire.beginTransmission(3);
      sendMatrix();
      Wire.beginTransmission(4);
      sendMatrix();
   }
   if (string != ""){
            Serial.println(string); //Output the message
           string =""; //clear the buffer/message
        }

// Keep reading from Arduino Serial Monitor and send to HC-05
  if (Serial.available())
    BTSerial.write(Serial.read());
}
void sendMatrix(){
    String buff = string.substring(4);
    char buffer[buff.length()];
    buff.toCharArray(buffer, buff.length());
    Wire.write(buffer);
    Wire.endTransmission();
    delay(500);
    digitalWrite(13,LOW);
    string = "";
      }
