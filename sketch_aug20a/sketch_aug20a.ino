/**
 * Simple routine that performs the following:
 *  1. Configures the software UART on pins 2 and 4 (RX,TX)
 *  2. Increments a 32-bit variable every 500ms
 *  4. If it receives a '1' character from bluetooth, it toggles an LED
 *     
 *  @author Justin Bauer - mcuhq.com
 *  @date 4.24.2016
 */

#include <SoftwareSerial.h> // use the software uart
#define UDP_TX_PACKET_MAX_SIZE 64
SoftwareSerial bluetooth(10, 11); // RX, TX
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  //buffer to hold incoming packet,      // a string to send back
String BTMsgArray[20];

void setup() {
  pinMode(13, OUTPUT); // for LED status
  Serial.begin(9600);
  pinMode(9,OUTPUT); digitalWrite(9,HIGH);
  bluetooth.begin(38400); // start the bluetooth uart at 9600 which is its default
  delay(1500);
  digitalWrite(13,!digitalRead(13));
  delay(750);
   digitalWrite(13,!digitalRead(13));
   delay(375);
    digitalWrite(13,!digitalRead(13));
    delay(375);// wait for settings to take affect. 
    Serial.println("Started");
}

void loop() {
  if (Serial.available()){
  bluetooth.println(Serial.read());
  }
  while (bluetooth.available()) { // check if anything in UART buffer
      char packetBuffer = bluetooth.read();
    }
    Serial.print(String(packetBuffer));
    digitalWrite(13,!digitalRead(13)); // if so, toggle the onboard LED
   }
