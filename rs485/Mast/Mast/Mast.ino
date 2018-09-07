#include <RS485_protocol.h>
#include <SoftwareSerial.h>
/*-----( Declare Constants and Pin Numbers )-----*/
#define SSerialRX        10  //Serial Receive pin
#define SSerialTX        11  //Serial Transmit pin

#define SSerialTxControl 3   //RS485 Direction control

#define RS485Transmit    HIGH
#define RS485Receive     LOW

#define Pin13LED         13

/*-----( Declare objects )-----*/
SoftwareSerial rs485(SSerialRX, SSerialTX); // RX, TX
void fWrite (const byte what)
  {
  rs485.write (what);  
  }
  
int fAvailable ()
  {
  return rs485.available ();  
  }

int fRead ()
  {
  return rs485.read ();  
  }
/*-----( Declare Variables )-----*/
int byteReceived;
int byteSend;

void setup()   /****** SETUP: RUNS ONCE ******/
{
  // Start the built-in serial port, probably to Serial Monitor
  Serial.begin(9600);
  // Start the software serial port, to another device
  rs485.begin(28800);   // set the data rate
  Serial.println("RS485 example");
  pinMode(Pin13LED, OUTPUT);   
  pinMode(SSerialTxControl, OUTPUT);    
  digitalWrite(SSerialTxControl, RS485Receive);  // Init Transceiver 
  digitalWrite(13,HIGH);
  delay(3000);
  digitalWrite(13,LOW);
}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  if (Serial.available())
  {
    byteReceived = Serial.read();   
    digitalWrite(SSerialTxControl, RS485Transmit);  // Enable RS485 Transmit   
    sendMsg(fWrite,byteReceived, sizeof byteReceived);
     while (!(UCSR0A & (1 << UDRE0)))  // Wait for empty transmit buffer
     UCSR0A |= 1 << TXC0;  // mark transmission not complete
 while (!(UCSR0A & (1 << TXC0)));   // Wait for the transmission to complete
    // Send byte to Remote Arduino
    digitalWrite(SSerialTxControl, RS485Receive);  // Disable RS485 Transmit       
    byte buf [10];
    byte received  = recvMsg(fAvailable, fRead, buf, sizeof buf);
    digitalWrite(Pin13LED, received == 0);
  } 

}//--(end main loop )---
