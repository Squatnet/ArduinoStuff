// Varidaic Debug Macro //
#define DEBUG   //Comment this line to disable Debug output
#ifdef DEBUG    // Debug is on
  #define DPRINT(...)    Serial.print(__VA_ARGS__)     //Sends our arguments to DPRINT()
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //Sends our arguments to DPRINTLN()
#else // Debug is off
  #define DPRINT(...)     //Nothing Happens
  #define DPRINTLN(...)   //Nothing Happens
#endif // end macro

// PJON stuff //
#define PJON_INCLUDE_SWBB
#include <PJONSlave.h>  // we are inslave mode .
uint8_t bus_id[] = {0, 0, 1, 53}; // Ancs unique bus ID
PJONSlave<SoftwareBitBang> bus(bus_id, PJON_NOT_ASSIGNED); // Force no id so master can assign us
int packet; // Holder for our packet
bool acquired = false; // did we get an address? 
bool debugMode = false; // Are we debugging
uint32_t t_millis; // tick tock
int ourID = 255; // Our ID number 

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
  Serial.flush(); // wait til serial is printed
};

// PJON RECEIVER CODE
void receiver_handler(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
   const char * arr = payload; // Not a pointer now.... !
  string.concat(arr); // addd it to our string
  if( string.startsWith("ack")){ // master got our registation message!
    ack = true; // nice
    string = ""; // thats all it says!
    DPRINT("Heard from server"); 
  }
  else parser(); // whats it say then ?? 
  // prints it to the console letter by letter
  DPRINT("Received: ");
  for(uint16_t i = 0; i < length; i++) {
    DPRINT((char)payload[i]);
    DPRINT(" ");
  }
  DPRINTLN();
  Serial.flush();
  
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
    
    DPRINTLN(string.length()); // prints the length of the command each iteration
  }
  DPRINT("STR = "); // prints after length < 1
  DPRINTLN(string);
  string = ""; // empty it
};
void setup() {  // SETUP 
  Serial.begin(115200); // Serial console
  DPRINT("Setup ");
  bus.set_error(error_handler); // link PJON to error handler
  DPRINT(". ");
  bus.set_receiver(receiver_handler); // link PJON to receiver
  DPRINT(". ");
  bus.strategy.set_pin(12); // Set PJON pin
  DPRINT(". ");
  bus.begin(); // 
  DPRINT(". ");
  delay(160); // possibly not needed if master is online
  DPRINT(". ");
  bus.acquire_id_master_slave(); //get an id
  DPRINT(". ");
  DPRINTLN("Done!!!");
  // SETUP FINISHES
}
//shows free ram
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
// Function to register with master.
// simply converts a String to a char array and sends it
void tellMasterAboutSelf(){
  String newPkt = "Reg,Type,Name"; // Edit this line
  const char pkt[newPkt.length()+1]; // Create array
  newPkt.toCharArray(pkt,newPkt.length()); // Convert string to Char[]
  bus.send(254,pkt,newPkt.length()+1); // Send the packet to master. 
}
void loop() {
  if(ourID == 255 && millis() > 15000){ // No id has been assigned and 15s have elapsed
    DPRINTLN("NO ID AFTER 15000");
    resetFunc(); // reset
  }
  if((bus.device_id() != PJON_NOT_ASSIGNED) && !acquired) { // we have an id, but havent regisrtered
    DPRINT("Acquired device id: ");
    DPRINTLN(bus.device_id()); 
    Serial.flush();
    delay(100);
    acquired = true; // track that
    tellMasterAboutSelf(); // and register
  }
  bus.update(); // update the PJON
  bus.receive(5000); // receive for a while
} //END
