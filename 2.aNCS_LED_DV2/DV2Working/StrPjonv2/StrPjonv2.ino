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

// FastLEDS stuff //
#include "FastLED.h" // The library
#define NUM_LEDS 28 // Leds per strip
#define DATA_PIN 2 // Strip 1 pin
#define DATA_PIN_2 3 // strip 2 pin
#define DATA_PIN_3 4 // strip 3 pin
#define DATA_PIN_4 5 // strip 4 pin
#define FRAMES_PER_SECOND  120 // for fastLEds dealy
#define CONNECTED_STRIPS 4 // possibly defunct
#define FL(aa,bb) for (int i = aa; i < bb; i++) // Clever for loop replacemetn
CRGB ledsA[NUM_LEDS]; // Strip 1 Array
CRGB ledsB[NUM_LEDS]; //  ""   2   ""
CRGB ledsC[NUM_LEDS]; //  ""   3   ""
CRGB ledsD[NUM_LEDS]; //  ""   4   ""
CRGB ourCol = CRGB(255, 255, 255); // Holder for our color
CRGB startup[] = {CRGB(255, 123, 0), CRGB(0, 255, 45), CRGB(0, 123, 255), CRGB(0, 255, 255)}; // Startup color array

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
unsigned long delayStart = 0; // the time the delay started
bool delayRunning = false; // true if still waiting for delay to finish


// Shows free ram 
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

// Program vars //
int x = 0; // holder for i2c message
String string = ""; // holder for our Pjon Message
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
int timeSinceBt = 0; // last message came when ?
int autoMode = 1; // Autopilot? 
int autoSecs = 10; // Autotimeout
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
    DPRINT("Message is ");
    DPRINT(string);
    DPRINT(" With length ");
    DPRINTLN(string.length());
    String subs = string.substring(0,string.indexOf(",")); // get everything until the first comma.
    string.remove(0,string.indexOf(0,string.indexOf(",")+1)); // remove everything up to and including the first comma
    if (subs.startsWith("Rst"))resetFunc(); // Reboot yourself. messge is destryed at this point
    if (subs.startsWith("ptn")){ // next value is pattern. 
      String ptn = string.substring(0,string.indexOf(",")); // get everything until the comma
      x = ptn.toInt(); // Its going to be an integer. its the pattern number,
      string.remove(0,string.indexOf(0,string.indexOf(",")+1)); // Remove the value
    }
    if (subs.startsWith("atm")){ // next value is boolean for automode
      String atm = string.substring(0,string.indexOf(",")); // get until first comma
      autoMode = atm.toInt(); // also an integer
      string.remove(0,string.indexOf(0,string.indexOf(",")+1)); // remove it
    }
    if (subs.startsWith("ats")){ // next value is autoSecs
      String ats = string.substring(0,string.indexOf(",")); // get the value, 
      autoSecs = ats.toInt();
      string.remove(0,string.indexOf(0,string.indexOf(",")+1)); // remove the value and trailing comma
    }
    if (subs.startsWith("col")){ // its the color
      String r = string.substring(0,string.indexOf(",")); // first bit is red, 
      ourCol.r = r.toInt(); // convert to an int
      string.remove(0,string.indexOf(0,string.indexOf(",")+1)); // remove red and comma
      String b = string.substring(0,string.indexOf(",")); // next up its blue
      ourCol.b = b.toInt(); // to integer
      string.remove(0,string.indexOf(0,string.indexOf(",")+1)); // remove blue and comma
      String g = string.substring(0,string.indexOf(",")); // then green 
      ourCol.g = g.toInt(); // conver to int
      string.remove(0,string.indexOf(0,string.indexOf(",")+1)); // thats colour done, remove the value and the comma
    }
    DPRINTLN(string.length()); // prints the length of the command each iteration
  }
  DPRINT("STR = "); // prints after length < 1
  DPRINTLN(string);
  string = ""; // empty it
}

void setup() {  // SETUP 
  Serial.begin(115200); // Serial console
  bus.set_error(error_handler); // link PJON to error handler
  bus.set_receiver(receiver_handler); // link PJON to receiver
  bus.strategy.set_pin(12); // Set PJON pin
  bus.begin(); // 
  delay(160); // possibly not needed if master is online
  bus.acquire_id_master_slave(); //get an id
  delayStart = millis();
  delayRunning = true;
  /* Initialize LED Strips */ 
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(ledsA, NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN_2, GRB>(ledsB, NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN_3, GRB>(ledsC, NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN_4, GRB>(ledsD, NUM_LEDS);
  FastLED.setBrightness(128);
  /* FINISH Init LED Strips */ 
  /* LED SPLASH COLORS */ 
  int i = 0;
  while (i < 4) {
    ourCol = startup[i];
    turnOn();
    copyLeds();
    FastLED.show();
    FastLED.delay(300);
    i++;
  }
  DPRINTLN("Done!!!");
  // SETUP FINISHES
}

// Function to register with master.
// simply converts a String to a char array and sends it
void tellMasterAboutSelf(){
  String newPkt = "Reg,Str, Any1 "; // Edit this line
  const char pkt[newPkt.length()+1]; // Create array
  newPkt.toCharArray(pkt,newPkt.length()); // Convert string to Char[]
  bus.send(254,pkt,newPkt.length()+1); // Send the packet to master. 
}
// picks a random pattern
void randX() { 
  if (autoMode == 1) {
    x = random(2, 9);
  }
}
// Copys the first strip to all others.
void copyLeds() {
  ///for (int i = 0; i < NUM_LEDS; i++) 
  FL(0,NUM_LEDS){
    ledsB[i] = ledsA[i];
    ledsC[i] = ledsA[i];
    ledsD[i] = ledsA[i];
  }
}
// Turns all leds off
void turnOff() {
  FL(0,NUM_LEDS) {
    ledsA[i] = CRGB( 0, 0, 0);
  }
}
// Set all LEDS to a specified color
void turnOn() {
  FL(0,NUM_LEDS) {
    ledsA[i] = ourCol;
  }
}

/* FAST LEDS PATTERNS */
void theLights() { //  speckles and strobes
  fadeToBlackBy( ledsA, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  ledsA[pos] = ourCol;
}
void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( ledsA, NUM_LEDS, gHue, 7);
}
void addGlitter( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    ledsA[ random16(NUM_LEDS) ] += CRGB::White;
  }
}
void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}
void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( ledsA, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  ledsA[pos] = ourCol;
  ledsA[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( ledsA, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS - 1 );
  ledsA[pos] = ourCol;
  ledsA[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  FL(0,NUM_LEDS) { //9948
    ledsA[i] = ourCol;
    ledsA[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( ledsA, NUM_LEDS, 20);
  byte dothue = 0;
  FL(0,8) {
    ledsA[beatsin16( i + 7, 0, NUM_LEDS - 1 )] |= ourCol;
    ledsA[beatsin16( i + 7, 0, NUM_LEDS - 1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
/* END FASTLEDS PATTERNS */

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
    Serial.flush();
    delay(100);
    ourID = bus.device_id();
    acquired = true; // track that
    tellMasterAboutSelf(); // and register
  }
  bus.update(); // update the PJON
  bus.receive(5000); // receive for a while
  EVERY_N_MILLISECONDS(30) {
    gHue++; // iterate thru colors
  }
  // autopilot timer
  EVERY_N_SECONDS(1) {
    timeSinceBt++;
    DPRINT("Free memory - ");
    DPRINTLN(freeRam());
    if (timeSinceBt == autoSecs) {
      timeSinceBt = 0;
      randX();
    }
  }
  // pattern chosen here
  switch (x) {
    case 0:
      turnOff();
      break;
    case 1:
      turnOn();
      break;
    case 2:
      theLights();
      break;
    case 3:
      rainbow();
      break;
    case 4:
      rainbowWithGlitter();
      break;
    case 5:
      confetti();
      break;
    case 6:
      sinelon();
      break;
    case 7:
      bpm();
      break;
    case 8:
      juggle();
      break;

  }
  FastLED.show(); // show the LEDS
  FastLED.delay(1000 / FRAMES_PER_SECOND); // insert a delay to keep the framerate modest

} //END
