#include <RS485_protocol.h>


/*-----( Import needed libraries )-----*/
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

/*-----( Declare Variables )-----*/
int byteReceived;
int byteSend;
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
void setup()   /****** SETUP: RUNS ONCE ******/
{
  // Start the built-in serial port, probably to Serial Monitor
  Serial.begin(9600);
  Serial.println("SerialRemote");  // Can be ignored
  pinMode(Pin13LED, OUTPUT);   
  pinMode(SSerialTxControl, OUTPUT);  
  digitalWrite(SSerialTxControl, RS485Receive);  // Init Transceiver
  
  // Start the software serial port, to another device
  rs485.begin(28800);   // set the data rate 
}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{ 
  byte buf [10];
  byte received = recvMsg(fAvailable,fRead,buf,sizeof (buf));
  //Copy input data to output  
  if (received) 
  {    
    digitalWrite(13,HIGH);
    digitalWrite(SSerialTxControl, RS485Transmit);  // Enable RS485 Transmit    
    sendMsg(fWrite,received,sizeof received); // Send the byte back 
    digitalWrite(SSerialTxControl, RS485Receive);  // Disable RS485 Transmit      
//    delay(100);
  }// End If RS485SerialAvailable
  
}//--(end main loop )---

/*-----( Declare User-written Functions )-----*/
//NONE

//*********( THE END )***********
