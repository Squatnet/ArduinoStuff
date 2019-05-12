/**
 * @todo Make slaves react to clock pulses and bpm changes
 * @body Slave devices should be ready to receive a float from master indicating a change in BPM, They should also run a function on receiving a clock pulse from master
 */
 
// Varidaic Debug Macro //
#define DEBUG   //Comment this line to disable Debug output
#ifdef DEBUG    // Debug is on
  #define DBEGIN(...)    Serial.begin(__VA_ARGS__)     // Debug serial begin
  #define DPRINT(...)    Serial.print(__VA_ARGS__)     //Sends our arguments to DPRINT()
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)  //Sends our arguments to DPRINTLN()
  #define DFLUSH(...)    Serial.flush() // Flush serial
#else // Debug is off
  #define DBEGIN(...)
  #define DPRINT(...)     //Nothing Happens
  #define DPRINTLN(...)
  #define DFLUSH(...)//Nothing Happens
#endif // end macro
// REGISTATION // 
// EDIT THIS //
String regString = "Reg,WNI,OSCIn "; // note the trailing space "Reg,Str,Left " , "Reg,Mat,Top ", "Reg,Strip,Right " //

// Ethernet & OSC Includes ///
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <OSCBundle.h>

// Ethernet Vars //
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xFE }; 
byte ip[] = { 192, 168, 1, 221 };
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet
unsigned int localPort = 9050; // OSC IN port
int ledPin =  13;       
int ledState = LOW;
EthernetUDP Udp; // init ETH UDP lib

// PJON stuff //
#define PJON_INCLUDE_SWBB
#define PJON_PIN  7
#include <PJONSlave.h>  // Coz we are inslave mode .
uint8_t bus_id[] = {0, 0, 1, 53}; // Ancs unique ID
PJONSlave<SoftwareBitBang> bus(bus_id, PJON_NOT_ASSIGNED); // Force no id so master can assign us
int packet; // Holder for our packet
bool acquired = false; // did we get an address? 
bool debugMode = false; // Are we debugging
uint32_t t_millis; // tick tock
int ourID = 255; // Our ID number 
unsigned long DELAY_TIME = 15000; // 1.5 sec
unsigned long delayStart = 0; // the time the delay started
bool delayRunning = false; // true if still waiting for delay to finish


// Shows free ram 
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
// Program vars //
String string = ""; // holder for our Pjon Message
bool ack = false; // Master has registered us?
void(* resetFunc) (void) = 0; // Software reset hack

// PJON error handling 
void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) { // Master is gone !!
    DPRINT("Connection lost with device ");
    DPRINTLN((uint8_t)bus.packets[data].content[0], DEC);
    delay(1000); // wait a second
    resetFunc(); // Reset
  }
  if(code == PJON_ID_ACQUISITION_FAIL) { // Didnt get an addres... !
    if(data == PJON_ID_ACQUIRE) {
      DPRINTLN("PJONSlave error: multi-master addressing failed.");
      // Didnt get id in Multi-Master environment
      delay(1000);
      resetFunc(); // bye!
    }
    if(data == PJON_ID_CONFIRM) // failed to confirm id with master... This shouldnt happen
      DPRINTLN("PJONSlave error: master-slave id confirmation failed.");
    if(data == PJON_ID_NEGATE)
      // We wont encounter this as we dont intend to give up out ID
      DPRINTLN("PJONSlave error: master-slave id release failed.");
    if(data == PJON_ID_REQUEST)
      // We couldnt find a Master on the network.... 
      DPRINTLN("PJONSlave error: master-slave id request failed.");
      delay(400); // wait 400ms
      if (millis() > 15000){ // if 15s has passed
        DPRINTLN("Resetting due to no ID");
        delay(300); // we reset
        resetFunc();
        } // if not
      bus.acquire_id_master_slave(); // try and get an id again
      acquired = false; // make sure we resend our registration when we reconnect
      delay(160); // makes the delay about 500ms between retrys
     
 }
  DFLUSH(); // wait til serial is printed
};

// PJON RECEIVER CODE
void receiver_handler(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
   const char * arr = payload; // Not a pointer now.... !
  string.concat(arr); // addd it to our string
  if( string.startsWith("ack")){ // master got our registation message!
    ack = true; // nice
    string = ""; // thats all it says!
    DPRINT("Heard from server : "); 
  }
  else parser(); // whats it say then ?? 
  // prints it to the console letter by letter
  DPRINT("Received: ");
  for(uint16_t i = 0; i < length; i++) {
    DPRINT((char)payload[i]);
    DPRINT(" ");
  }
  DPRINTLN();
  DFLUSH();
  
};
// Reads an incoming control message
void parser(){
  while(string.length() >= 1){ // While there message left to read. 
    String subs = string.substring(0,string.indexOf(",")); // get everything until the first comma.
    string.remove(0,string.indexOf(0,string.indexOf(",")+1)); // remove everything up to and including the first comma
    if (subs.startsWith("Rst"))resetFunc(); // Reboot yourself. messge is destryed at this point
    /* 
     * Add your if staments here, Strips pattern code added for example
     * if (subs.startsWith("ptn")){ // next value is pattern. 
     *   String ptn = string.substring(0,string.indexOf(",")); // get everything until the comma
     *   x = ptn.toInt(); // Its going to be an integer. its the pattern number,
     *   string.remove(0,string.indexOf(0,string.indexOf(",")+1)); // Remove the value
     *   }
     *  
     */
   
   else {
      string.remove(0,string.indexOf(",")+1);
    DPRINTLN(string);
    DPRINTLN(string.length());
    }// prints the length of the command each iteration
  }
  DPRINT("STR = "); // prints after length < 1
  DPRINTLN(string);
  string = ""; // empty it
};
void iic(String x){
  //Wire.beginTransmission(addr);
  char c[x.length()+1];
  x.toCharArray(c,x.length()+1);
  //Wire.write(c);
  bus.send(254,c,x.length()+1);
  bus.update();
  DPRINTLN("Sent Pjon");
  //Wire.endTransmission();
}
void CLK(OSCMessage &msg, int addrOffset ){ // get float from clock #1 - Receive int/pulse
  int b = msg.getFloat(0);
  DPRINT("CLK = : ");
  DPRINTLN(b);
  String toSend = "Clk,";
  toSend.concat(String(b));
  iic(toSend); // send the clock value/pulse to pjon.
}
void KICK(OSCMessage &msg, int addrOffset ){ // get float from clock #1 - Receive int/pulse
  int b = msg.getFloat(0);
  if(b !=  0){  
    DPRINT("KICK = : ");
    DPRINTLN(b);
    String toSend = "Kick,";
    toSend.concat(String(b));
    iic(toSend); // send the clock value/pulse to pjon.
  }
}
void SNARE(OSCMessage &msg, int addrOffset ){ // get float from clock #1 - Receive int/pulse
  int b = msg.getFloat(0);
    if(b !=  0){  
    DPRINT("Snare = : ");
    DPRINTLN(b);
    String toSend = "Snare,";
    toSend.concat(String(b));
    iic(toSend); // send the clock value/pulse to pjon.
  }
}
void HH(OSCMessage &msg, int addrOffset ){ // get float from clock #1 - Receive int/pulse
  int b = msg.getFloat(0);
  if(b != 0){
    DPRINT("HH = : ");
    DPRINTLN(b);
    String toSend = "HiHat,";
    toSend.concat(String(b));
    iic(toSend); // send the clock value/pulse to pjon.
  }
}
void HHO(OSCMessage &msg, int addrOffset ){ // get float from clock #1 - Receive int/pulse
  int b = msg.getFloat(0);
  if(b != 0){
    DPRINT("HH = : ");
    DPRINTLN(b);
    String toSend = "HiHat,";
    toSend.concat(String(b));
    iic(toSend); // send the clock value/pulse to pjon.
  }
}
void IO(OSCMessage &msg, int addrOffset){ // Get float from message
  ledState = (boolean) msg.getFloat(0); // get the float and change the bool state
  digitalWrite(ledPin, ledState); // write led state
  if (ledState) { // debug - turn osc I/O
    DPRINTLN("OSC on");
    iic(ledState);
  }
  else {
    DPRINTLN("OSC off");
    iic(ledState);
  }
  ledState = !ledState;     // change led state
}
void setup() {  // SETUP 
  DBEGIN(115200); // Serial console
  DPRINT("Setup ");
  bus.set_error(error_handler); // link PJON to error handler
  DPRINT(". ");
  bus.set_receiver(receiver_handler); // link PJON to receiver
  DPRINT(". ");
  bus.strategy.set_pin(PJON_PIN); // Set PJON pin
  DPRINT(". ");
  bus.begin(); // 
  DPRINT(". ");
  delay(160); // possibly not needed if master is online
  DPRINT(". ");
  bus.acquire_id_master_slave(); //get an id
  DPRINT(". ");
  delayStart = millis();
  DPRINT(". ");
  delayRunning = true;
  DPRINT(". ");
  pinMode(4, OUTPUT); // disable SD
  DPRINT(". ");
  digitalWrite(4, HIGH);
  DPRINT(". ");
  Ethernet.begin(mac, ip);  // init ETH
  DPRINT(". ");
  DPRINTLN("Done!!!");
  DPRINTLN("Arduino IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    DPRINT(Ethernet.localIP()[thisByte], DEC);
    DPRINT("."); 
  }
  DPRINTLN();
  Udp.begin(localPort); // listen to port 9000
  // SETUP FINISHES
};
// Function to register with master.
// simply converts a String to a char array and sends it
void tellMasterAboutSelf(){ 
  const char pkt[regString.length()+1]; // Create array
  regString.toCharArray(pkt,regString.length()); // Convert string to Char[]
  bus.send(254,pkt,regString.length()+1); // Send the packet to master. 
};
void loop() {
    if (delayRunning && ((millis() - delayStart) >= DELAY_TIME)) {
      if(bus.device_id() == PJON_NOT_ASSIGNED){
       DPRINTLN("NO ID AFTER 15000");
       DFLUSH();
       resetFunc(); // reset
    }
    if(!acquired | !ack){
      DPRINTLN("Check master knows about");
      DFLUSH();
      bus.send(254,"Chk,",5);
      acquired = true;
      ack = true;
    }
    delayStart += DELAY_TIME; // No id has been assigned and 15s have elapsed
  }
  if((bus.device_id() != PJON_NOT_ASSIGNED) && !acquired) { // we have an id, but havent regisrtered
    DPRINT("Acquired device id: ");
    DPRINTLN(bus.device_id()); 
    DFLUSH();
    delay(100);
    acquired = true; // track that
    tellMasterAboutSelf(); // and register
  }
  OSCBundle bundleIN; // init bundle class
  int size; 
  if( (size = Udp.parsePacket())>0) { // if the packet is not null
    while(size--) 
      bundleIN.fill(Udp.read()); // read packet
    DPRINTLN("INTERCEPTED OSC");
    if(!bundleIN.hasError()) // if in the list below:
      DPRINTLN(" INTERESTED IN OSC ");
      bundleIN.route("/aCK4", CLK);
      bundleIN.route("/lKick", KICK);
      bundleIN.route("/lSnare", SNARE);
      bundleIN.route("/lHH", HH);
      bundleIN.route("/lHHo", HHO);
      bundleIN.route("/Test", DPRINTLN("TEST"));
   }
  bus.update(); // update the PJON
  bus.receive(5000); // receive for a while
} //END
