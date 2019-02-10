// Varidaic Debug Macro
//#define DEBUG   //Comment this line to disable Debug output
#ifdef DEBUG    // Debug is on
  #define DPRINT(...)    Serial.print(__VA_ARGS__)     //Sends our arguments to DPRINT()
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //Sends our arguments to DPRINTLN()
  #define DFLUSH(...)    Serial.flush()
#else // Debug is off
  #define DPRINT(...)     //Nothing Happens
  #define DPRINTLN(...)   //Nothing Happens
  #define DFLUSH(...)
#endif // end macro
//DHCP-based OSC server test code
#include <Wire.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <OSCBundle.h>
#define I2C_ADDR 10
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xFD }; 
byte ip[] = { 192, 168, 1, 200 };
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet
unsigned int localPort = 9000; // OSC IN port
int ledPin =  13;       
int ledState = LOW;
bool active = false;
String string = "";
EthernetUDP Udp; // init ETH UDP lib
void receiveEvent(int howMany) {
  Serial.print("gotMessage  ");
  while (Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    string.concat(c);       // print the character
  }
  Serial.println(string);
  string.trim();
  parser();
}
void parser(){
  while(string.length() >= 1){ // While there message left to read. 
    DPRINT("Message is ");
    DPRINT(string);
    DPRINT(" With length ");
    if(!string.endsWith(","))string.concat(","); // STOP CRASHING;
    DPRINTLN(string.length());
    String subs = string.substring(0,string.indexOf(",      ")); // get everything until the first comma.
    string.remove(0,string.indexOf(0,string.indexOf(",")+1)); // remove everything up to and including the first comma
    if(subs.startsWith("Mod")){
      if(active)active = false;
      else active=true;
    }
    DPRINTLN(string.length()); // prints the length of the command each iteration
  }
  DPRINT("STR = "); // prints after length < 1
  DPRINTLN(string);
  string = ""; // empty it
}
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
  Wire.begin(I2C_ADDR);
  Wire.onReceive(receiveEvent);
}

void loop(){
  if(active){
 OSCBundle bundleIN; // init bundle class
 int size; 
  if( (size = Udp.parsePacket())>0) { // if the packet is not null
    while(size--)bundleIN.fill(Udp.read()); // read packet
    if(!bundleIN.hasError()) // if in the list below:
      bundleIN.route("/lKick", KICK);
      bundleIN.route("/lSnare", SNARE);
      bundleIN.route("/lHH", HH);
      bundleIN.route("/lHHo", HHO);
      }
   } 
}
void iic(int addr ,String x){
  Wire.beginTransmission(addr);
  char c[x.length()+1];
  x.toCharArray(c,x.length()+1);
  Wire.write(c);
  Serial.println("Sent I2c");
  Wire.endTransmission();
}
void KICK(OSCMessage &msg, int addrOffset ){ // get float from clock #1 - Receive int/pulse
  int b = msg.getFloat(0);
  if(b !=  0){  
    Serial.print("KICK = : ");
    Serial.println(b);
    String toSend = "Kick,";
    toSend.concat(String(b));
    iic(1,toSend); // send the clock value/pulse to pjon.
  }
}
void SNARE(OSCMessage &msg, int addrOffset ){ // get float from clock #1 - Receive int/pulse
  int b = msg.getFloat(0);
    if(b !=  0){  
    Serial.print("Snare = : ");
    Serial.println(b);
    String toSend = "Snare,";
    toSend.concat(String(b));
    iic(2,toSend); // send the clock value/pulse to pjon.
  }
}
void HH(OSCMessage &msg, int addrOffset ){ // get float from clock #1 - Receive int/pulse
  int b = msg.getFloat(0);
  if(b != 0){
    Serial.print("HH = : ");
    Serial.println(b);
    String toSend = "HiHat,";
    toSend.concat(String(b));
    iic(3,toSend); // send the clock value/pulse to pjon.
  }
}
void HHO(OSCMessage &msg, int addrOffset ){ // get float from clock #1 - Receive int/pulse
  int b = msg.getFloat(0);
  if(b != 0){
    Serial.print("HH = : ");
    Serial.println(b);
    String toSend = "HiHat,";
    toSend.concat(String(b));
    iic(4,toSend); // send the clock value/pulse to pjon.
  }
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
