/*
 * 
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
String regString = "Reg,WNI,Clock "; // note the trailing space "Reg,Str,Left " , "Reg,Mat,Top ", "Reg,Strip,Right " //
// i2c stuff //
#include <Wire.h>
#define I2CADDR  8
// PJON stuff //
#define PJON_INCLUDE_SWBB
#include <PJONSlave.h>  // Coz we are inslave mode .
uint8_t bus_id[] = {0, 0, 1, 53}; // Ancs unique ID
PJONSlave<SoftwareBitBang> bus(bus_id, PJON_NOT_ASSIGNED); // Force no id so master can assign us
int packet; // Holder for our packet
bool acquired = false; // did we get an address? 
bool debugMode = false; // Are we debugging
uint32_t t_millis; // tick tock
int ourID = 255; // Our ID number 
unsigned long DELAY_TIME = 15000; // 1.5 sec
unsigned long DELAY_2_TIME = 10000;
unsigned long delayStart = 0; // the time the delay started
unsigned long delay2Start = 0;
bool delayRunning = false; // true if still waiting for delay to finish
bool delay2Running = false;


// Shows free ram 
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
// Program vars //
String string = ""; // holder for our Pjon Message
String sendStr = "";
int ticks = 0;
float bpm1 = 0;
float bpm2 = 0;
float bpm3 = 0;
float bpm4 = 0;
float bpm = 0;
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
void sendMaster(String str){ 
  DPRINT("SENDING TO MASTER");
  const char pkt[str.length()+1]; // Create array
  str.toCharArray(pkt,str.length()+1); // Convert string to Char[]
  bus.send(254,pkt,str.length()+1); // Send the packet to master. 
  DPRINTLN(pkt);
};
void receiveEvent(int howMany) {
  DPRINTLN("GOT I2C");
  while (Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a characte  DONT GIVE A FUCK.
  }
  ticks++; // count the messages
  DPRINT("Tick ");
  DPRINTLN(ticks);
}

void setup() {  // SETUP 
  DBEGIN(115200); // Serial console
  DPRINT("Setup ");
  Wire.begin(8);              // join i2c bus with address #8
  DPRINT(". ");
  Wire.onReceive(receiveEvent);
  DPRINT(". ");
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
  delayStart = millis();
  delayRunning = true;
  delay2Start = millis();
  delay2Running = true;
  DPRINTLN("Done!!!");
  
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
  if (delay2Running && ((millis() - delay2Start) >= DELAY_2_TIME)) {
      DPRINTLN("Computing BPM");
      DPRINT("TPS READ : ");
      float ticksPerSec = float (ticks / 10.0);
      ticks = 0;
      DPRINT(ticksPerSec);
      bpm2 = bpm1;
      bpm1 = bpm;
      DPRINT(" - NEW BPM : ");
      bpm = float(ticksPerSec * 60);
      if(bpm1 == 0)bpm1 = bpm;
      if(bpm2 == 0)bpm2 = bpm;
      DPRINTLN(bpm);
      float avgBPM = ( (bpm + bpm1 + bpm2) / 3 );
      String msg = "Clk,";
      msg.concat(String(avgBPM));
      msg.concat(",");
      sendMaster(msg);
      delay2Start += DELAY_2_TIME;
      }
  if((bus.device_id() != PJON_NOT_ASSIGNED) && !acquired) { // we have an id, but havent regisrtered
    DPRINT("Acquired device id: ");
    DPRINTLN(bus.device_id()); 
    DFLUSH();
    delay(100);
    acquired = true; // track that
    tellMasterAboutSelf(); // and register
  }
  bus.update(); // update the PJON
  bus.receive(5000); // receive for a while
} //END