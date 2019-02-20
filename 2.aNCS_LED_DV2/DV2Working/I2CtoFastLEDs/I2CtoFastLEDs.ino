// Varidaic Debug Macro
#define DEBUG   //Comment this line to disable Debug output
#ifdef DEBUG    // Debug is on
#define DBEGIN(...)    Serial.begin(__VA_ARGS__)
#define DPRINT(...)    Serial.print(__VA_ARGS__)     //Sends our arguments to DPRINT()
#define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //Sends our arguments to DPRINTLN()
#define DFLUSH(...)    Serial.flush()
#else // Debug is off
#define DBEGIN(...)
#define DPRINT(...)     //Nothing Happens
#define DPRINTLN(...)   //Nothing Happens
#define DFLUSH(...)
#endif // end macro

#define DEBUG_LED 13
#include "FastLED.h"
#include <Wire.h>
#define FRAMES_PER_SECOND  120
#define ZOOMING_BEATS_PER_MINUTE 122
#define STROBE_BEATS_PER_MINUTE 97.5
#define NUM_STRIPS 3
#define NUM_LEDS_PER_STRIP 29
#define NUM_LEDS NUM_LEDS_PER_STRIP * NUM_STRIPS
#define I2C_ADDR 1
#define FL(aa,bb) for (int i = aa; i < bb; i++)
CRGB leds[NUM_LEDS];
CRGB ourCol = CRGB(255, 255, 255);
CRGB startup[] = {CRGB(255, 123, 0), CRGB(0, 255, 45), CRGB(0, 123, 255), CRGB(0, 255, 255)};

String string = "";
uint8_t gHue = 0;
int x = 0; // holder for i2c message
int y = 0;
int timeSinceBt = 0;
int autoMode = 1;
int autoSecs = 2;
int stripNumber = 3;
int individualStripMode = 1;
int LEDStart = 0;
int LEDEnd = 0;
int NoLEDs = 0;
int patternStore[NUM_STRIPS+1];
bool debugLED = false;

void(* resetFunc) (void) = 0; // Software reset hack
// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  DPRINT("gotMessage  ");
  while (Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    string.concat(c);       // print the character
  }
  timeSinceBt = 0;
  DPRINTLN(string);
  string.trim();
  parser();
}
void setLEDs() {
  if (individualStripMode == 1) {
    NoLEDs = NUM_LEDS_PER_STRIP;
    FL(1, NUM_STRIPS + 1) {

      if ((individualStripMode == 1) && (stripNumber == i)) {
        LEDEnd = (NUM_LEDS_PER_STRIP * i);
        LEDStart = ((NUM_LEDS_PER_STRIP * i) - (NUM_LEDS_PER_STRIP));
      }
    }
  }
  if (individualStripMode == 0) {
    NoLEDs = NUM_LEDS;
    stripNumber = 0;
    LEDStart = 0;
    LEDEnd = NUM_LEDS;
  }
}
void turnOn() {
  FL(LEDStart, LEDEnd) {
    leds[i] = ourCol;
  }
}
void turnOff() {
  FL(LEDStart, LEDEnd) {
    leds[i] = CRGB( 0, 0, 0);
  }
}
void parser() {
  DPRINTLN("PARSER");
  while (string.length() >= 1) { // While there message left to read.
    DPRINT("Message is ");
    DPRINT(string);
    DPRINT(" With length ");
    DPRINTLN(string.length());
    if (!string.endsWith(","))string.concat(","); // STOP CRASHING;
    String subs = string.substring(0, string.indexOf(',')); // get everything until the first comma.
    DPRINT("SubStr - ");
    DPRINT(subs);
    string.remove(0, string.indexOf(',') + 1); // remove everything up to and including the first comma
    DPRINT(" String - ");
    DPRINTLN(string);
    if (subs.startsWith("Rst"))resetFunc();
    if (subs.startsWith("Pul")) {
      doPulse();
      string = "";
    }
    if (subs.startsWith("SNo")) { // next value is strip No. to address.
      DPRINT("Strip No. ");
      String sno = string.substring(0, string.indexOf(",")); // get everything until the comma
      DPRINT(sno);
      DPRINT(" - ");
      stripNumber = sno.toInt(); // Its going to be an integer.
      DPRINTLN(stripNumber);
      string.remove(0, string.indexOf(",") + 1); // Remove the value
    setLEDs();
  
  }
    if (subs.startsWith("ISt")) { // individual Strip Mode. next value if 0, addresses all strips, if 1 addresses strips individually.
      DPRINT("individual Strip Mode ");
      String ist = string.substring(0, string.indexOf(",")); // get everything until the comma
      DPRINT(ist);
      DPRINT(" - ");
      individualStripMode = ist.toInt(); // Its going to be an integer.
      DPRINTLN(individualStripMode);
      string.remove(0, string.indexOf(",") + 1); // Remove the value
    if (individualStripMode==0){
      setLEDs();
    }
    }
    if (subs.startsWith("Ptn")) { // next value is pattern.
      DPRINT("PTN ");
      String ptn = string.substring(0, string.indexOf(",")); // get everything until the comma
      DPRINT(ptn);
      DPRINT(" - ");
      x = ptn.toInt(); // Its going to be an integer. its the pattern number,
      DPRINTLN(x);
      string.remove(0, string.indexOf(",") + 1); // Remove the value
    if(individualStripMode==1){
      patternStore[stripNumber]=x;
    }
    }
    if (subs.startsWith("Atm")) { // next value is boolean for automode
      DPRINT("ATM ");
      String atm = string.substring(0, string.indexOf(",")); // get until first comma
      autoMode = atm.toInt(); // also an integer
      DPRINT(atm);
      DPRINT(" - ");
      DPRINTLN(autoMode);
      string.remove(0, string.indexOf(",") + 1); // remove it
    }
    if (subs.startsWith("Ats")) { // next value is autoSecs
      DPRINT("ATS ");
      String ats = string.substring(0, string.indexOf(",")); // get the value,
      autoSecs = ats.toInt();
      string.remove(0, string.indexOf(",") + 1); // remove the value and trailing comma
    }
    if (subs.startsWith("Col")) { // its the color
      String r = string.substring(0, string.indexOf(",")); // first bit is red,
      ourCol.r = r.toInt(); // convert to an int
      string.remove(0, string.indexOf(",") + 1); // remove red and comma
      String b = string.substring(0, string.indexOf(",")); // next up its blue
      ourCol.b = b.toInt(); // to integer
      string.remove(0, string.indexOf(",") + 1); // remove blue and comma
      String g = string.substring(0, string.indexOf(",")); // then green
      ourCol.g = g.toInt(); // conver to int
      string.remove(0, string.indexOf(",") + 1); // thats colour done, remove the value and the comma
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
  FL(LEDStart, LEDEnd) {
    leds[i] = CRGB::White;
  }
  FastLED.show();
  FastLED.delay(100);
  turnOff();
  FastLED.show();
}
void randX() {
  if (autoMode == 1) {
  if (individualStripMode==0){
    x = random(2, 9);
    DPRINT("RANDOM ");
    DPRINTLN(x);
  }
  if (individualStripMode!=0){
      patternStore[stripNumber]=random(2,9);
  }
  }
}
void theLights() { //  speckles and strobes
  fadeToBlackBy(&(leds[LEDStart]), NoLEDs, 10);
  int pos = random16(LEDStart, LEDEnd);
  leds[pos] = ourCol;
}
void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow(&(leds[LEDStart]), NoLEDs, 7);
}
void addGlitter( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    leds[ random16(LEDStart, LEDEnd) ] += CRGB::White;
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
  fadeToBlackBy(&(leds[LEDStart]), NoLEDs, 10);
  int pos = random16(LEDStart, LEDEnd);
  leds[pos] = ourCol;
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}
void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(&(leds[LEDStart]), NoLEDs, 20);
  int pos = beatsin16( 13, LEDStart, LEDEnd - 1 );
  leds[pos] = ourCol;
  leds[pos] += CHSV( gHue, 255, 192);
}
void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  FL(LEDStart, LEDEnd) { //9948
    leds[i] = ourCol;
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}
void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(&(leds[LEDStart]), NoLEDs, 20);
  byte dothue = 0;
  FL(0, 8) {
    leds[beatsin16( i + 7, LEDStart, LEDEnd - 1 )] |= ourCol;
    leds[beatsin16( i + 7, LEDStart, LEDEnd - 1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
void simpleStrobe () {
  fill_solid(&(leds[LEDStart]), NoLEDs, CRGB::Black);
  const uint8_t kStrobeCycleLength = 6; // light every Nth frame
  static uint8_t sStrobePhase = 0;
  sStrobePhase = sStrobePhase + 1;
  if ( sStrobePhase >= kStrobeCycleLength ) {
    sStrobePhase = 0;
  }
  if ( sStrobePhase == 0 ) {
    uint8_t dashperiod = beatsin8( 8/*cycles per minute*/, 4, 10);
    uint8_t dashwidth = (dashperiod / 4) + 1;
    uint8_t zoomBPM = STROBE_BEATS_PER_MINUTE;
    int8_t  dashmotionspeed = beatsin8( (zoomBPM / 2), 1, dashperiod);
    if ( dashmotionspeed >= (dashperiod / 2)) {
      dashmotionspeed = 0 - (dashperiod - dashmotionspeed );
    }
    uint8_t cycle = beat8(2); // two cycles per minute
    uint8_t easedcycle = ease8InOutCubic( ease8InOutCubic( cycle));
    uint8_t wavecycle = cubicwave8( easedcycle);
    // uint8_t hueShift = 0; // NO SHIFT OF HUE IN COLOUR (we should rebuild in RGB...)
    uint8_t hueShift = scale8( wavecycle, 130); // METHOD HOW HUE VALUE SHIFTS
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
  if ( sRepeatCounter >= stroberepeats) {
    sRepeatCounter = 0;
    sStartPosition = sStartPosition + dashmotionspeed;
    if ( sStartPosition >= dashperiod ) {
      while ( sStartPosition >= dashperiod) {
        sStartPosition -= dashperiod;
      }
      sStartHue  -= huedelta;
    } else if ( sStartPosition < LEDEnd) {
      while ( sStartPosition < LEDEnd) {
        sStartPosition += dashperiod;
      }
      sStartHue  += huedelta;
    }
  }
  const uint8_t kSaturation = 208; // WHITE >> CURRENT COLOUR control (def 208)
  const uint8_t kValue = 200; // Brightness??
  strobeDraw( sStartPosition, LEDEnd - 1, dashperiod, dashwidth, sStartHue, huedelta, kSaturation, kValue);
}
static void strobeDraw(uint8_t startpos, uint16_t lastpos, uint8_t period, uint8_t width,  uint8_t huestart, uint8_t huedelta, uint8_t saturation, uint8_t value)
{
  uint8_t hue = huestart;
  for ( uint16_t i = LEDStart; i <= LEDEnd; i += period) {
    CRGB color = CHSV( hue, saturation, value);
    //CRGB color = CRGB::Blue; // USE TO COMPLETELY BYPASS HSV Change Scheme
    uint16_t pos = i;
    for ( uint8_t w = 0; w < width; w++) {
      leds[ pos ] = color;
      pos++;
      if ( pos >= LEDEnd) {
        break;
      }
    }
    hue += huedelta;
  }
}
void patternSelect(){
  int lastStripNumber=stripNumber;
  if (individualStripMode==0){
    setLEDs();
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
  }
  if (individualStripMode!=0){
    FL(1,NUM_STRIPS+1){
      stripNumber=i;
      setLEDs();
      y=patternStore[i];
      switch (y) {
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
    stripNumber=lastStripNumber;
    }
  }
}
void setup() {
  Wire.begin(I2C_ADDR);
  Wire.onReceive(receiveEvent);
  pinMode(DEBUG_LED, OUTPUT);
  DBEGIN(115200);
  DPRINTLN("Ready for i2c");
  //sets one long array containing multiple data pins in the following format.
  //type of led/ data pin/ color order(if not RGB)/ name/ point in array to start adding from/ number of LED's to add.
  FastLED.addLeds<WS2812B, 2, GRB>(leds, 0, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, 3, GRB>(leds, NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, 4, GRB>(leds, 2 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);

  FastLED.setBrightness(128);
  int i = 0;
  while (i < NUM_STRIPS+1) {
    ourCol = startup[i];
    setLEDs();
    turnOn();
    FastLED.show();
    FastLED.delay(300);
    i++;
  }
  turnOff();
  FastLED.show();
  FL(1,NUM_STRIPS+1){
    patternStore[i]=i+1;
  } 
}
void loop() {
  EVERY_N_SECONDS(2)  // this flashes the onboard LED when loop is completed.
    if(debugLED){
      randX();
      debugLED = false;
      digitalWrite(DEBUG_LED,HIGH);
    }
    else{
      randX();
      debugLED = true;
      digitalWrite(DEBUG_LED,LOW);
    }
  if (autoMode != 2) {
    EVERY_N_MILLISECONDS(30) {
      gHue++;
    if (gHue>=255){
      gHue=0;
    }
   }
    EVERY_N_SECONDS(1) {      
      timeSinceBt++;
      if (timeSinceBt == autoSecs) {
    timeSinceBt = 0;}
    }
  }
  patternSelect();
    FastLED.show();
    FastLED.delay(1000 / FRAMES_PER_SECOND);
  
}
