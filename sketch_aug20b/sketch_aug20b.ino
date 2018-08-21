#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX
#define UDP_TX_PACKET_MAX_SIZE 64
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
void setup() {

Serial.begin(38400);

pinMode(9,OUTPUT); digitalWrite(9,HIGH);

mySerial.begin(38400);
char Mess;

}

void loop()

{

while (mySerial.available()){
  unsigned char data = mySerial.read();
  Serial.println(data, HEX);

}
if (Serial.available())

mySerial.println(Serial.read());


}
