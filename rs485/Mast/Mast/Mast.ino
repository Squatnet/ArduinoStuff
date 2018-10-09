
/*-----( Import needed libraries )-----*/
#include <SoftwareSerial.h>
#include <Wire.h>
/*-----( Declare Constants and Pin Numbers )-----*/
#define RX_A       4  //Serial Receive pin
#define TX_A       5 //Serial Transmit pin
#define TX_ON_A    3
#define RX_B       7
#define TX_B       8
#define TX_ON_B    6
#define RS485Transmit    HIGH
#define RS485Receive     LOW
#define i2cNum     1
#define Pin13LED         13

/*-----( Declare objects )-----*/
SoftwareSerial RS485A(RX_A, TX_A); // RX, TX
SoftwareSerial RS485B(RX_B, TX_B); // Side 2 RX,TX
/*-----( Declare Variables )-----*/
char byteReceived;
char byteSend;
String stringeh = "";
void receiveEvent(int howMany) {
  while (0 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
    stringeh.concat(c);
  }
}
void setup()   /****** SETUP: RUNS ONCE ******/
{
  // Start the built-in serial port, probably to Serial Monitor
  Serial.begin(9600);
  Serial.println("Nano RS485 Master serial console");
  pinMode(Pin13LED, OUTPUT);   
  digitalWrite(TX_ON_A, RS485Receive);  // Init Transceiver A
  digitalWrite(TX_ON_B, RS485Receive);  // Init Transceiver B
  Wire.begin(1);
  Serial.print("RS485 A,RS485 B, i2c setup");
  // Start the software serial port, to another device
  RS485A.begin(38400); 
  RS485B.begin(38400);// set the data rate 
  Serial.print(" ....");
  Serial.println(".... Done!");

}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  digitalWrite(Pin13LED, HIGH);  // Show activity
  while(Serial.available()){
     char character = Serial.read(); // Receive a single character from the software serial port
        stringeh.concat(character); // Add the received character to the receive buffer
  }
  if (stringeh != ""){
    if(stringeh.startsWith("A")){
     digitalWrite(TX_ON_A,RS485Transmit);
    String buff = stringeh.substring(1);
    char buffer[buff.length()];
    buff.toCharArray(buffer, buff.length());
     RS485A.write(buffer);
     digitalWrite(TX_ON_A,RS485Receive);
     stringeh = "";
    }
    else if (stringeh.startsWith("B")){
      digitalWrite(TX_ON_B,RS485Transmit);
    String buff = stringeh.substring(1);
    char buffer[buff.length()];
    buff.toCharArray(buffer, buff.length());
      RS485B.write(buffer);
      }
      digitalWrite(TX_ON_B,RS485Receive);
      stringeh = "";
    }
    else {
      Serial.println("UNKNOWN FILTER SENDING TO ALL");
    digitalWrite(TX_ON_A,RS485Transmit);
    digitalWrite(TX_ON_A,RS485Transmit);
    String buff = stringeh.substring(1);
    char buffer[buff.length()];
    buff.toCharArray(buffer, buff.length());
     RS485A.write(buffer);
     RS485B.write(buffer);
     digitalWrite(TX_ON_A,RS485Receive);
     digitalWrite(TX_ON_A,RS485Receive);
     stringeh = "";
  }
}//--(end main loop )---
