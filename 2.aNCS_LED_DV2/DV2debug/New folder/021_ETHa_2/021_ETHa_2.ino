#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <OSCBundle.h>

byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xFD }; 
byte ip[] = { 192, 168, 1, 221 };
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet
unsigned int localPort = 9050; // OSC IN port
EthernetUDP Udp; // init ETH UDP lib

void setup() {
  Serial.begin(9600); 
  Serial.println("Setup");
  Serial.print("Arduino IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }
  pinMode(4, OUTPUT); // disable SD
  digitalWrite(4, HIGH);
  Ethernet.begin(mac, ip);  // init ETH
  
  
}
void loop() {
  Serial.println("loop start");
OSCBundle bundleIN; // init bundle class
  int size; 
  if( (size = Udp.parsePacket())>0) { // if the packet is not null
    Serial.println("udp != 0 yay!!");
    while(size--) 
      bundleIN.fill(Udp.read()); // read packet
    
    if(!bundleIN.hasError()) // if in the list below:
    Serial.println("got here");
      bundleIN.route("/aCK4", CLK);
      bundleIN.route("/lKick", KICK);
      }
  Serial.print("loop End");
} //END
void CLK(OSCMessage &msg, int addrOffset ){ // get float from clock #1 - Receive int/pulse
  int b = msg.getFloat(0);
  Serial.print("CLK = : ");
  Serial.println(b);
  }
void KICK(OSCMessage &msg, int addrOffset ){ // get float from clock #1 - Receive int/pulse
  int b = msg.getFloat(0);
  if(b !=  0){  
    Serial.print("KICK = : ");
    Serial.println(b);
    }
}
void SNARE(OSCMessage &msg, int addrOffset ){ // get float from clock #1 - Receive int/pulse
  int b = msg.getFloat(0);
    if(b !=  0){  
    Serial.print("Snare = : ");
    Serial.println(b);
    }
}
void HH(OSCMessage &msg, int addrOffset ){ // get float from clock #1 - Receive int/pulse
  int b = msg.getFloat(0);
  if(b != 0){
    Serial.print("HH = : ");
    Serial.println(b);
    }
}
void HHO(OSCMessage &msg, int addrOffset ){ // get float from clock #1 - Receive int/pulse
  int b = msg.getFloat(0);
  if(b != 0){
    Serial.print("HHo = : ");
    Serial.println(b);
    }
}
