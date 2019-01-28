//DHCP-based OSC server test code
#include <Wire.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <OSCBundle.h>
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xFD }; 
byte ip[] = { 192, 168, 1, 200 };
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet
unsigned int localPort = 9000; // OSC IN port
int ledPin =  13;       
int ledState = LOW;
EthernetUDP Udp; // init ETH UDP lib

void setup(){
  Serial.begin(115200); 
  Serial.println("Setup");
  pinMode(4, OUTPUT); // disable SD
  digitalWrite(4, HIGH);
  Ethernet.begin(mac, ip);  // init ETH
  
  Serial.print("Arduino IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print("."); 
  }
  Udp.begin(localPort); // listen to port 9000
  Wire.begin(); //i2c
}

void loop(){
 OSCBundle bundleIN; // init bundle class
 int size; 
  if( (size = Udp.parsePacket())>0) { // if the packet is not null
    while(size--) 
      bundleIN.fill(Udp.read()); // read packet
    if(!bundleIN.hasError()) // if in the list below:
      bundleIN.route("/Test", Test); // runtimes
      bundleIN.route("/aCK4", CLK);
   }
} 

void iic(int addr ,int x){
  Wire.beginTransmission(addr);
  Wire.write(x);
  Serial.println("Sent I2c");
  Wire.endTransmission();
}

void Test(OSCMessage &msg, int addrOffset ){ // get test value
  int a = msg.getFloat(0); // get the fucking float info.
  Serial.print("Value = : ");
  Serial.println(a);
  iic(8,a); // send the clock value/pulse to pjon.
}

void CLK(OSCMessage &msg, int addrOffset ){ // get float from clock #1 - Receive int/pulse
  int b = msg.getFloat(0);
  Serial.print("Value = : ");
  Serial.println(b);
  iic(8,b); // send the clock value/pulse to pjon.
}

void IO(OSCMessage &msg, int addrOffset){ // Get float from message
  ledState = (boolean) msg.getFloat(0); // get the float and change the bool state
  digitalWrite(ledPin, ledState); // write led state
  if (ledState) { // debug - turn osc I/O
    Serial.println("OSC on");
    iic(8,ledState);
  }
  else {
    Serial.println("OSC off");
    iic(8,ledState);
  }
  ledState = !ledState;     // change led state
}
/*
 * To send an OSC message...
 * 
 * OSCMessage msgOUT("/Fader/Value");

  msgOUT.add(value);

  Udp.beginPacket(Udp.remoteIP(), destPort);
  msgOUT.send(Udp); // send the bytes
  Udp.endPacket(); // mark the end of the OSC Packet
  msgOUT.empty(); // free space occupied by message
 * 
 */
