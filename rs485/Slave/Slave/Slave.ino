
/*-----( Import needed libraries )-----*/
#include <SoftwareSerial.h>
#include <Wire.h>

/*-----( Declare Constants and Pin Numbers )-----*/
#define SSerialRX        10  //Serial Receive pin
#define SSerialTX        11  //Serial Transmit pin

#define SSerialTxControl 3   //RS485 Direction control
#define RS485Transmit    HIGH
#define RS485Receive     LOW

#define Pin13LED         13

/*-----( Declare objects )-----*/
SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // RX, TX

/*-----( Declare Variables )-----*/
int byteReceived;
char byteSend;

String Name = "";

void setup()   /****** SETUP: RUNS ONCE ******/
{
  // Start the built-in serial port, probably to Serial Monitor
  Serial.begin(9600);
  Serial.println("Serial  Remote");  // Can be ignored
  Wire.begin();
  pinMode(Pin13LED, OUTPUT);   
  pinMode(SSerialTxControl, OUTPUT);  
  
  digitalWrite(SSerialTxControl, RS485Receive);  // Init Transceiver
  
  // Start the software serial port, to another device
  RS485Serial.begin(38400);   // set the data rate 
}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  //Copy input data to output  
  while (RS485Serial.available()) 
  {
    byteSend = RS485Serial.read();   // Read the byte 
    Name.concat(byteSend);
    digitalWrite(Pin13LED, HIGH);  // Show activity             
    digitalWrite(Pin13LED, LOW);
    Serial.print(byteSend);
    digitalWrite(SSerialTxControl, RS485Transmit);  // Enable RS485 Transmit    
    RS485Serial.write(byteSend); // Send the byte back
    digitalWrite(SSerialTxControl, RS485Receive);  // Disable RS485 Transmit      
//    delay(100);
  }// End If RS485SerialAvailable
  if(Name!=""){
    Wire.beginTransmission(8);
    char buffa[Name.length()+1] ;
    Name.toCharArray(buffa,Name.length()+1);
    Wire.write(buffa);
    Wire.endTransmission();
    Serial.println(Name);
    Name = "" ;
  }
  
}//--(end main loop )---
