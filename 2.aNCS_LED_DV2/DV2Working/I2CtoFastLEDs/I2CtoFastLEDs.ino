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
#define DEBUG_LED 13
#include "FastLED.h"
#include <Wire.h>
#define NUM_LEDS 28
#define DATA_PIN 2
#define DATA_PIN_2 3
//#define DATA_PIN_3 4
//#define DATA_PIN_4 5
#define FRAMES_PER_SECOND  120
#define ZOOMING_BEATS_PER_MINUTE 122
#define STROBE_BEATS_PER_MINUTE 97.5
#define I2C_ADDR 1
#define CONNECTED_STRIPS 2
#define FL(aa,bb) for (int i = aa; i < bb; i++)
CRGB ledsA[NUM_LEDS];
CRGB ledsB[NUM_LEDS];
//CRGB ledsC[NUM_LEDS];
//CRGB ledsD[NUM_LEDS];
CRGB ourCol = CRGB(255, 255, 255);
CRGB startup[] = {CRGB(255, 123, 0), CRGB(0, 255, 45), CRGB(0, 123, 255), CRGB(0, 255, 255)};
int x = 0; // holder for i2c message
String string = "";
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
int timeSinceBt = 0;
int autoMode = 1;
int autoSecs = 2;
bool debugLED = false;
void(* resetFunc) (void) = 0; // Software reset hack
// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  Serial.print("gotMessage  ");
  while (Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    string.concat(c);       // print the character
  }
  timeSinceBt = 0;
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
    String subs = string.substring(0,string.indexOf(",")); // get everything until the first comma.
    string.remove(0,string.indexOf(",")+1); // remove everything up to and including the first comma
    if(subs.startsWith("Pul")){
      doPulse();
      string = "";
    }
    if (subs.startsWith("Ptn")){ // next value is pattern. 
      String ptn = string.substring(0,string.indexOf(",")); // get everything until the comma
      x = ptn.toInt(); // Its going to be an integer. its the pattern number,
      string.remove(0,string.indexOf(",")+1); // Remove the value
    }
    if (subs.startsWith("Atm")){ // next value is boolean for automode
      String atm = string.substring(0,string.indexOf(",")); // get until first comma
      autoMode = atm.toInt(); // also an integer
      string.remove(0,string.indexOf(",")+1); // remove it
    }
    if (subs.startsWith("Ats")){ // next value is autoSecs
      String ats = string.substring(0,string.indexOf(",")); // get the value, 
      autoSecs = ats.toInt();
      string.remove(0,string.indexOf(",")+1); // remove the value and trailing comma
    }
    if (subs.startsWith("Col")){ // its the color
      String r = string.substring(0,string.indexOf(",")); // first bit is red, 
      ourCol.r = r.toInt(); // convert to an int
      string.remove(0,string.indexOf(",")+1); // remove red and comma
      String b = string.substring(0,string.indexOf(",")); // next up its blue
      ourCol.b = b.toInt(); // to integer
      string.remove(0,string.indexOf(",")+1); // remove blue and comma
      String g = string.substring(0,string.indexOf(",")); // then green 
      ourCol.g = g.toInt(); // conver to int
      string.remove(0,string.indexOf(",")+1); // thats colour done, remove the value and the comma
    }
    DPRINTLN(string.length()); // prints the length of the command each iteration
  }
  DPRINT("STR = "); // prints after length < 1
  DPRINTLN(string);
  string = ""; // empty it
}
void doPulse() {
  turnOff();
  FastLED.show();
  FL(0,NUM_LEDS){
    ledsA[i]=CRGB::White;
  }
  copyLeds();
  FastLED.show();
  FastLED.delay(100);
  turnOff();
  FastLED.show();
}
void randX() {
  if (autoMode == 1) {
    x = random(2, 10);
    Serial.print("RANDOM ");
    Serial.println(x);
  }
}
void setup() {
  Wire.begin(I2C_ADDR);
  Wire.onReceive(receiveEvent);
  Serial.begin(115200);
  pinMode(DEBUG_LED, OUTPUT);
  Serial.println("Ready for i2c");
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(ledsA, NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN_2, GRB>(ledsB, NUM_LEDS);
  //FastLED.addLeds<WS2812B, DATA_PIN_3, GRB>(ledsC, NUM_LEDS);
  //FastLED.addLeds<WS2812B, DATA_PIN_4, GRB>(ledsD, NUM_LEDS);
  FastLED.setBrightness(128);
  int i = 0;
  while (i < 4) {
    ourCol = startup[i];
    turnOn();
    copyLeds();
    FastLED.show();
    FastLED.delay(300);
    i++;
  }
  turnOff();
  copyLeds();
  FastLED.show();
}
void loop() {
  EVERY_N_SECONDS(2) {
    if(debugLED){
      debugLED = false;
      digitalWrite(DEBUG_LED,HIGH);
    }
    else{
      debugLED = true;
      digitalWrite(DEBUG_LED,LOW);
    }
  }
  if(autoMode != 2){
    EVERY_N_MILLISECONDS(30) {
      gHue++;
    }
    EVERY_N_SECONDS(1) {
      timeSinceBt++;
      if (timeSinceBt == autoSecs) {
       timeSinceBt = 0;
       randX();
      }
    }
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
      case 9:
        simpleStrobe();
        break;

    }
    //call function
    copyLeds();

    FastLED.show();
    FastLED.delay(1000 / FRAMES_PER_SECOND); // insert a delay to keep the framerate modest

    //  EVERY_N_MILLISECONDS( 20 )
  }
}
void copyLeds() {
  ///for (int i = 0; i < NUM_LEDS; i++) 
  FL(0,NUM_LEDS){
    ledsB[i] = ledsA[i];
  //  ledsC[i] = ledsA[i];
  //  ledsD[i] = ledsA[i];
  }
}
void turnOff() {
  FL(0,NUM_LEDS) {
    ledsA[i] = CRGB( 0, 0, 0);
  }
}
void turnOn() {
  FL(0,NUM_LEDS) {
    ledsA[i] = ourCol;
  }
}
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
void simpleStrobe () {
 fill_solid( ledsA, NUM_LEDS, CRGB::Black);
 const uint8_t kStrobeCycleLength = 6; // light every Nth frame
  static uint8_t sStrobePhase = 0;
  sStrobePhase = sStrobePhase + 1;
  if( sStrobePhase >= kStrobeCycleLength ) { 
    sStrobePhase = 0; 
  }
 if( sStrobePhase == 0 ) {
    uint8_t dashperiod= beatsin8( 8/*cycles per minute*/, 4,10);
    uint8_t dashwidth = (dashperiod / 4) + 1;
    uint8_t zoomBPM = STROBE_BEATS_PER_MINUTE;
    int8_t  dashmotionspeed = beatsin8( (zoomBPM /2), 1,dashperiod);
    if( dashmotionspeed >= (dashperiod/2)) { 
      dashmotionspeed = 0 - (dashperiod - dashmotionspeed );
    }
    uint8_t cycle = beat8(2); // two cycles per minute
    uint8_t easedcycle = ease8InOutCubic( ease8InOutCubic( cycle));
    uint8_t wavecycle = cubicwave8( easedcycle);
   // uint8_t hueShift = 0; // NO SHIFT OF HUE IN COLOUR (we should rebuild in RGB...) 
    uint8_t hueShift = scale8( wavecycle,130); // METHOD HOW HUE VALUE SHIFTS
    uint8_t strobesPerPosition = 2; // try 1..4
    strobeCore( dashperiod, dashwidth, dashmotionspeed, strobesPerPosition, hueShift);
  }
}
void strobeCore( 
    uint8_t dashperiod, uint8_t dashwidth, int8_t  dashmotionspeed, uint8_t stroberepeats,
    uint8_t huedelta) {
  static uint8_t sRepeatCounter = 0;
  static int8_t sStartPosition = 0;
  static uint8_t sStartHue = 0;
  sStartHue += 1; //Shift the Colour little by little
  sRepeatCounter = sRepeatCounter + 1;
  if( sRepeatCounter>= stroberepeats) {
    sRepeatCounter = 0;
    sStartPosition = sStartPosition + dashmotionspeed;
    if( sStartPosition >= dashperiod ) {
      while( sStartPosition >= dashperiod) { sStartPosition -= dashperiod; }
      sStartHue  -= huedelta;
    } else if( sStartPosition < 0) {
      while( sStartPosition < 0) { sStartPosition += dashperiod; }
      sStartHue  += huedelta;
    }
  }
  const uint8_t kSaturation = 208; // WHITE >> CURRENT COLOUR control (def 208)
  const uint8_t kValue = 200; // Brightness??
  strobeDraw( sStartPosition, NUM_LEDS-1, dashperiod, dashwidth, sStartHue, huedelta, kSaturation, kValue);
}
static void strobeDraw( 
  uint8_t startpos, uint16_t lastpos, uint8_t period, uint8_t width, 
  uint8_t huestart, uint8_t huedelta, uint8_t saturation, uint8_t value) 
  {
  uint8_t hue = huestart;
  for( uint16_t i = startpos; i <= lastpos; i += period) {
    CRGB color = CHSV( hue, saturation, value);  
    //CRGB color = CRGB::Blue; // USE TO COMPLETELY BYPASS HSV Change Scheme
    uint16_t pos = i;
    for( uint8_t w = 0; w < width; w++) {
      ledsA[ pos ] = color;
      pos++;
      if( pos >= NUM_LEDS) {
        break;
      }
    }
    hue += huedelta;
  }
}

//function where a red pixle moves down the LED strips from A1-D28. RE Issue #44
void pixleRef1(){
  FL(0,NUM_LEDS){
    ledsA[i]=CRGB::Red;
    ledsA[i-1]=ourCol;
    FastLED.show();
    delay(250);
    if (i==28){
      ledsA[28]=ourCol;
      }
  }
      FL(0,NUM_LEDS){
        ledsB[i]=CRGB::Red;
        ledsB[i-1]=ourCol;
        FastLED.show();
        delay(250);
        if (i==28){
          ledsB[28]=ourCol;
          }
      }
      /*FL(0,NUM_LEDS){
        ledsC[i]=CRGB::Red;
        ledsC[i-1]=ourCol;
        FastLED.show();
        delay(250);
        if (i==28){
          ledsC[28]=ourCol;
          }
      }
      FL(0,NUM_LEDS){
        ledsD[i]=CRGB::Red;
        ledsD[i-1]=ourCol;
        FastLED.show();
        delay(250);
        if (i==28){
          ledsD[28]=ourCol;
          }
      }*/
}
