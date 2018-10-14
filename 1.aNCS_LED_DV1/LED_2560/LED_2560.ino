#include <FastLED.h>
#include <Wire.h>
#include <LEDMatrix.h>
#include <LEDText.h>
#include <FontMatrise.h>

#define FL(aa,bb) for (int i = aa; i < bb; i++)

#define LED_PIN        3
#define COLOR_ORDER    GRB
#define CHIPSET        WS2812B
#define MATRIX_WIDTH   32
#define MATRIX_HEIGHT  8
#define NUM_LEDS       256
#define MATRIX_TYPE    HORIZONTAL_ZIGZAG_MATRIX
#define FRAMES_PER_SECOND  120
#define STROBE_BEATS_PER_MINUTE 97.5

#define I2C_ADDR 8
#define CONNECTED_STRIPS 1

cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;
cLEDText ScrollingMsg;
CRGB tst[256];
CRGB ourCol = CRGB(255, 255, 255);
const unsigned char TxtANCS[] = { 
  EFFECT_FRAME_RATE "\x02"
  EFFECT_HSV_AV "\x00\xff\xff\x40\x00\xff"
  EFFECT_SCROLL_LEFT "    ANCS - CUSTOM BUILT AUDIO / VISUAL - WWW.ANCS.GQ    " 
  };
/*
unsigned char txtLive[] = "";
unsigned char displayTxt = {  
    EFFECT_FRAME_RATE "\x02" 
    EFFECT_HSV_AV "\x00\xff\xff\x40\x00\xff" 
    EFFECT_SCROLL_LEFT ... // issue starts here - Consider various options
  };
  */
int iic = 0; // holder for i2c message
String string = "";
int timeSinceBt = 0;
int autoMode = 1;
int autoSecs = 10;  
         
void setup() {
  Wire.begin(2); //2,3,4 for A,B+C aNCS_2560 boards.
  //Serial.begin(9600);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds[0], leds.Size());
  FastLED.setBrightness(64); // Limit Power Consumption 
  FastLED.clear(true);
  ScrollingMsg.SetFont(MatriseFontData); // or Robotron.
  ScrollingMsg.Init(&leds, leds.Width(), ScrollingMsg.FontHeight() + 1, 0, 0);
  ScrollingMsg.SetText((unsigned char *)TxtANCS, sizeof(TxtANCS) - 1);
  ScrollingMsg.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 0xff, 0x00, 0xff);
  delay(500);
  FastLED.showColor(CRGB::Red);
  FastLED.show();
}

typedef void (*PatternList[])();

PatternList gPatterns = { turnOff, turnOn, theLights, rainbow, rainbowWithGlitter, confetti, sinelon, bpm, juggle} ; 

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void requestEvent() {
  String msg = "STR,";
  msg += String(I2C_ADDR);
  msg += ",";
  msg += String(CONNECTED_STRIPS);
  msg += ",";
  msg += String(ourCol.r);
  msg += ",";
  msg += String(ourCol.g);
  msg += ",";
  msg += String(ourCol.b);
  msg += ",";
  msg += String(iic);
  msg += ",";
  msg += String(autoMode);
  msg += ",";
  msg += String(autoSecs);
  msg += ",";
  // add string of characters here - send to txtBuffer
  msg += '\0';
  char buffer[msg.length()];
  msg.toCharArray(buffer, msg.length());
  Serial.println(msg);
  Serial.println(msg.length());
  Wire.write(buffer);
  Serial.println(buffer);
}
//***I2c Reveive event***
void receiveEvent(int howMany) {
  while (Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    string.concat(c);       
  }
  timeSinceBt = 0;
  string.trim();
  String ss = string.substring(0, 1);
  string.remove(0, 1);
  if (ss == "X") {
    autoMode = string.toInt();
    Serial.print("AUTO");
    Serial.print(autoMode);
  }
  else if (ss == "S") {
    autoSecs = string.toInt();
    Serial.print("AUTOSECS = ");
    Serial.println(autoSecs);
  }
  else {
    iic = ss.toInt();
    if (string.length() == 6) {
      char charbuf[8];
      string.toCharArray(charbuf, 8);
      long int rgb = strtol(charbuf, 0, 16); //=>rgb=0x001234FE;
      byte r = (byte)(rgb >> 16);
      byte g = (byte)(rgb >> 8);
      byte b = (byte)(rgb);
      ourCol = CRGB(r, g, b);
    }
  }
  string = "";
}
void randX() {
  if (autoMode == 1) {
    iic = random(2, 9);
    Serial.print("RANDOM ");
    Serial.println(iic);
  }
}

/****************************************************** LOOP ***********************************************/  
void loop() {
    EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
    EVERY_N_SECONDS(1) {
      timeSinceBt++;
      if (timeSinceBt == autoSecs) {
        timeSinceBt = 0;
        randX();
      }
    }
    FL(0,10){
      if(iic==i){
        gCurrentPatternNumber=i;
      break;
      }
    }
    gPatterns[gCurrentPatternNumber](); // Call the current pattern function once, updating the 'leds' array
    FastLED.show();  
    FastLED.delay(1000/FRAMES_PER_SECOND); // insert a delay to keep the framerate modest 
    // Periodic Updates
        //EVERY_N_SECONDS( 25 ) { nextPattern(); } // will change for menu selection switch/case style selection.
}


/******************************************************* PATTERNS  *************************************************************************/

/*
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
void nextPattern() {
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns); // add one to the current pattern number, and wrap around at the end
}
*/
// No longer used, Call Pattern via string data.

void turnOff() {
  FL(0,NUM_LEDS) {
    tst[i] = CRGB( 0, 0, 0);
    rXY();
  }
}
void turnOn() {
  FL(0,NUM_LEDS) {
    tst[i] = ourCol;
    rXY();
  }
}
void rXY() {
  for (int i=0;i<256;i++){
    leds(i) = tst[i]; // nMans best botch ever. Oi, f*ck off, be yourself.
   }
}

void scrollText(){
  if (ScrollingMsg.UpdateText() == -1) // if end of text
    ScrollingMsg.SetText((unsigned char *)TxtANCS, sizeof(TxtANCS) - 1); 
  else
    return;
    }

void solidWhite(){
  fill_solid( tst, NUM_LEDS, CRGB::Grey );
  rXY();
}
/*COLOURED SPECKLES PRESET LIST*/
void theLights() { //  speckles and strobes
  fadeToBlackBy( tst, 256, 10);
  int pos = random16(256);
  tst[pos] += CRGB::White;
  rXY();
}
/*****TEST*****/
void forestLights() { //  speckles and strobes
  uint8_t woodindex = 128;
  CRGBPalette16 palette = ForestColors_p;
  fadeToBlackBy( tst, 256, 10);
  int pos = random16(256);
  tst[pos] += ColorFromPalette( palette, woodindex);
  rXY();
}


void rainbow() {
  fill_rainbow( tst, NUM_LEDS, gHue, 7); // FastLED's built-in rainbow generator
  rXY();
}
void rainbowWithGlitter() {
  rainbow(); // built-in FastLED rainbow, plus some random sparkly glitter
  addGlitter(80); // Cool Glitter function (see Below)
  rXY();
}
void addGlitter( fract8 chanceOfGlitter) {
  if( random8() < chanceOfGlitter) {
    tst[ random16(NUM_LEDS) ] += CRGB::White;
  }
}
void rainbowLights() { //  speckles and strobes
  uint8_t rainbowindex = 128;
  CRGBPalette16 palette = RainbowColors_p;
  fadeToBlackBy( tst, 256, 10);
  int pos = random16(256);
  tst[pos] += ColorFromPalette( palette, rainbowindex);
  rXY();
}
void confetti() { // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( tst, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  tst[pos] += CHSV( gHue + random8(64), 200, 255);
  rXY();
}

void sinelon() { // a colored dot sweeping back and forth, with fading trails 
  fadeToBlackBy( tst, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  tst[pos] += CHSV( gHue, 255, 192);
  rXY();
}
void sineForest() { 
  uint8_t sinFindex = 128;
  CRGBPalette16 palette = ForestColors_p; 
  fadeToBlackBy( tst, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  tst[pos] += ColorFromPalette( palette, sinFindex);
  rXY();
}


void juggle() { // eight colored dots, weaving in and out of sync with each other 
  fadeToBlackBy( tst, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    tst[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    rXY();
    dothue += 32;
  }
}

void bpm() { // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 97;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    tst[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
  rXY();
}
void bpmLava() { // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 97;
  CRGBPalette16 palette = LavaColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    tst[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
  rXY();
}

 /********************************************** STROBE **********************************************/
 
/* TEST for RGB based Strobe - Unusual Behaviour atm, use HSV */ 
void basicStrobe () {
 fill_solid( tst, NUM_LEDS, CRGB::Black);
 const uint8_t kStrobeCycleLength = 4; // light every Nth frame
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
    int8_t strobesPerPosition = 1; // try 1..4
    basicStrobeCore( dashperiod, dashwidth, dashmotionspeed, strobesPerPosition);
  }
}
void basicStrobeCore( 
    uint8_t dashperiod, uint8_t dashwidth, int8_t  dashmotionspeed, uint8_t stroberepeats) {
  static uint8_t sRepeatCounter = 0;
  static int8_t sStartPosition = 0;
  sRepeatCounter = sRepeatCounter + 1;
  if( sRepeatCounter>= stroberepeats) {
    sRepeatCounter = 0;
    sStartPosition = sStartPosition + dashmotionspeed;
    if( sStartPosition >= dashperiod ) {
      while( sStartPosition >= dashperiod) { 
        sStartPosition -= dashperiod; }
    } else if( sStartPosition < 0) {
      while( sStartPosition < 0) { 
        sStartPosition += dashperiod; }
    }
  }
  basicStrobeDraw( sStartPosition, NUM_LEDS-1, dashperiod, dashwidth);
  // rXY(); might belong here rather than in basicStrobeDraw... test
}
static void basicStrobeDraw( 
  uint8_t startpos, uint16_t lastpos, uint8_t period, uint8_t width) 
  {
  for( uint16_t i = startpos; i <= lastpos; i += period) {
    /************** USE CASE STATEMENT TO CHANGE COLOURS *************/
    // switch(var) {
    //    case 0:
    //    CRGB color = CRGB::Red;
    //    break; ... etc etc
    CRGB color = CRGB::White; 
    uint16_t pos = i;
    for( uint8_t w = 0; w < width; w++) {
      tst[ pos ] = color;
      //rXY();
      pos++;
      if( pos >= NUM_LEDS) {
        break;
      }
      rXY();
    }
  }
}
/****************************************/
void oceanStrobe () {
 fill_solid( tst, NUM_LEDS, CRGB::Black);
 const uint8_t kStrobeCycleLength = 3; // light every Nth frame
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
    int8_t strobesPerPosition = 1; // try 1..4
    oceanStrobeCore( dashperiod, dashwidth, dashmotionspeed, strobesPerPosition);
  }
}
void oceanStrobeCore( 
    uint8_t dashperiod, uint8_t dashwidth, int8_t  dashmotionspeed, uint8_t stroberepeats) {
  static uint8_t sRepeatCounter = 0;
  static int8_t sStartPosition = 0;
  sRepeatCounter = sRepeatCounter + 1;
  if( sRepeatCounter>= stroberepeats) {
    sRepeatCounter = 0;
    sStartPosition = sStartPosition + dashmotionspeed;
    if( sStartPosition >= dashperiod ) {
      while( sStartPosition >= dashperiod) { 
        sStartPosition -= dashperiod; }
    } else if( sStartPosition < 0) {
      while( sStartPosition < 0) { 
        sStartPosition += dashperiod; }
    }
  }
  oceanStrobeDraw( sStartPosition, NUM_LEDS-1, dashperiod, dashwidth);
  // rXY(); might belong here rather than in basicStrobeDraw... test
}
static void oceanStrobeDraw( 
  uint8_t startpos, uint16_t lastpos, uint8_t period, uint8_t width) 
  {
  for( uint16_t i = startpos; i <= lastpos; i += period) {
    CRGBPalette16 palette = OceanColors_p; 
    uint16_t pos = i;
    for( uint8_t w = 0; w < width; w++) {
      tst[ pos ] = ColorFromPalette(palette, gHue);
      //rXY();
      pos++;
      if( pos >= NUM_LEDS) {
        break;
      }
      rXY();
    }
  }
}


