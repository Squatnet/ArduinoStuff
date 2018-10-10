
/*-----( Import needed libraries )-----*/
#include <SoftwareSerial.h>
#include <Wire.h>
/*-----( Declare Constants and Pin Numbers )-----*/
#define SSerialRX        11  //Serial Receive pin
#define SSerialTX        10  //Serial Transmit pin

#define SSerialTxControl 3   //RS485 Direction control
#define RS485Transmit    HIGH
#define RS485Receive     LOW

#define Pin13LED         13
String buff = "";
/*-----( Declare objects )-----*/
SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // RX, TX

/*-----( Declare Variables )-----*/


void setup()   /****** SETUP: RUNS ONCE ******/
{
  // Start the built-in serial port, probably to Serial Monitor
  Serial.begin(9600);
  Serial.println("Serial RS485 Remote");  // Can be ignored
  
  pinMode(Pin13LED, OUTPUT);   
  pinMode(SSerialTxControl, OUTPUT);  
  
  digitalWrite(SSerialTxControl, RS485Receive);  // Init Transceiver
  
  // Start the software serial port, to another device
  RS485Serial.begin(38400);   // set the data rate 
  Wire.begin();
}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  //Copy input data to output  
  while (RS485Serial.available()) 
  {
    char c = RS485Serial.read(); // receive byte as a character
    buff.concat(c);  
  }// End If RS485SerialAvailable
  if (buff != ""){
    Wire.beginTransmission(8);
    Serial.println("Recieved this From Rs485");
    Serial.println(buff);
    char buffer[buff.length()];
     buff.toCharArray(buffer, buff.length());
    Wire.write(buffer);
     Wire.endTransmission();
     buff = "";
  Serial.print("Sent Message");
  }
  }//--(end main loop )---
