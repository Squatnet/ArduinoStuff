#include "FastLED.h"
FASTLED_USING_NAMESPACE
#include <QueueArray.h>

#define DATA_PIN    3
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    256
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120
#define ZOOMING_BEATS_PER_MINUTE 122
#define STROBE_BEATS_PER_MINUTE 97.5

CRGB cArray[8] = { CRGB::White,CRGB::Red,CRGB::Blue,CRGB::Green,CRGB::Magenta,CRGB::Yellow,CRGB::Pink,CRGB::Brown };

const uint8_t kMatrixWidth = 32;
const uint8_t kMatrixHeight = 8;
const bool    kMatrixSerpentineLayout = true;

uint16_t XY( uint8_t x, uint8_t y) {
  uint16_t i;
  if( kMatrixSerpentineLayout == false) {
    i = (y * kMatrixWidth) + x;
  }
  if( kMatrixSerpentineLayout == true) {
    if( y & 0x01) {
      uint8_t reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;
    } else {
      i = (y * kMatrixWidth) + x;
    }
  }
  return i;
}

String txtIN = "STRONG" ;
int colourList[] = {0,1,2,3,4,5};
QueueArray <char> queue;
QueueArray <int> colours;
char txt; // queueArray popped character from string. Global so accessable.
int col; // queueArrray popped integer from string. Global.
boolean runOnce = false; // runTime controller for printing String once only.
int stringLength ; // Holder for the returned value of addLength(x)
int curStartStr; // Holder for current starting position of the stringLength first co-ordinate.

/***************************************** SETUP *********************************************/
void setup() {
  delay(3000); // 3 second delay for recovery
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}
/************************************* PATTERN PLAYLIST *************************************************/
typedef void (*PatternList[])(); // List of patterns to cycle through.  Each is defined as a separate function below.
//PatternList gPatterns = { rainbow, rainbowWithGlitter, theLights, confetti, sinelon, buildUp, simpleStrobe, juggle, bpm, strobe }; // Add more Patterns Here!
//PatternList gPatterns = { buildUp , theLights  } ;
PatternList gPatterns = {  theLights, runtime, } ;

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
/****************************************************** LOOP ***********************************************/  
void loop() {
    gPatterns[gCurrentPatternNumber](); // Call the current pattern function once, updating the 'leds' array
    FastLED.show();  
    FastLED.delay(1000/FRAMES_PER_SECOND); // insert a delay to keep the framerate modest 
    // Periodic Updates
    EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
    EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
}
/******************************************************* PATTERNS  *************************************************************************/
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
void nextPattern() {
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns); // add one to the current pattern number, and wrap around at the end
}
/****************************************************** WORD CONSTRUCTS (Replace with read string) **********************************************/
void runtime() {
  if(runOnce == false) {
    fill_solid( leds, NUM_LEDS, CRGB::Black);
    reSet();
    queueCol();
    queueStr();
    printStr();  
    runOnce = true;
  }
}
void reSet() {
  stringLength = 0; // Reset from leds[0]
}
void shiftString () {
// stringLength = 0 - curStartPos;  // Reset with Scrolling 
}
void addLength(int len){ // function takes the length of current character
  len = len+1 ; // add spacer after each character
stringLength += len; // compound operator (StrLen = StrLen + len)
return stringLength; // return the sum of total value
}
void queueStr() { // build a queue of the current text in string.
  for (int i = 0; i < txtIN.length (); i++){
    queue.enqueue (txtIN.charAt (i));
  }
}
void queueCol() {   
  for (int j = 0; j < sizeof(colourList); j++){
    colours.enqueue (colourList[j]);
  }
}
void printStr () {
  int pos ; // co-ordinate for drawing character
  while (!queue.isEmpty () ) { 
   txt = queue.pop();
   col = colours.pop(); 
   switch(txt) {
    case 'A':
    { pos = stringLength; addLength(4); lA(pos,col); }
    break;
    case 'B':
    { pos = stringLength; addLength(4); lB(pos,col); }
    break;
    case 'C':
    { pos = stringLength; addLength(4); lC(pos,col); }
    break;
    case 'D':
    { pos = stringLength; addLength(4); lD(pos,col); }
    break;
    case 'E':
    { pos = stringLength; addLength(4); lE(pos,col); }
    break;
    case 'F':
    { pos = stringLength; addLength(4); lF(pos,col); }
    break;
    case 'G':
    { pos = stringLength; addLength(4); lG(pos,col); }
    break;
    case 'H':
    { pos = stringLength; addLength(4); lH(pos,col); }
    break;
    case 'I':
    { pos = stringLength; addLength(5); lI(pos,col); }
    break;
    case 'J':
    { pos = stringLength; addLength(4); lJ(pos,col); }
    break;
    case 'K':
    { pos = stringLength; addLength(4); lK(pos,col); }
    break;
    case 'L':
    { pos = stringLength; addLength(4); lL(pos,col); }
    break;
    case 'M':
    { pos = stringLength; addLength(5); lM(pos,col); }
    break;
    case 'N':
    { pos = stringLength; addLength(5); lN(pos,col); }
    break;
    case 'O':
    { pos = stringLength; addLength(4); lO(pos,col); }
    break;
    case 'P':
    { pos = stringLength; addLength(4); lP(pos,col); }
    break;
    case 'Q':
    { pos = stringLength; addLength(4); lQ(pos,col); }
    break;
    case 'R':
    { pos = stringLength; addLength(5); lR(pos,col); }
    break;
    case 'S':
    { pos = stringLength; addLength(4); lS(pos,col); }
    break;
    case 'T':
    { pos = stringLength; addLength(5); lT(pos,col); }
    break;
    case 'U':
    { pos = stringLength; addLength(4); lU(pos,col); }
    break;
    case 'V':
    { pos = stringLength; addLength(5); lV(pos,col); }
    break;
    case 'W':
    { pos = stringLength; addLength(5); lW(pos,col); }
    break;
    case 'X':
    { pos = stringLength; addLength(5); lX(pos,col); }
    break;
    case 'Y':
    { pos = stringLength; addLength(4); lY(pos,col); }
    break;
    case 'Z':
    { pos = stringLength; addLength(5); lZ(pos,col); }
    break;
    }
  }
}
/****************************************************** ALPHABET AND SPECIAL CHARACTERS *******************************************************/
void lA (int x , int col) {
  yTXT( x , 0 , 7 , col);
  yTXT( (x+4), 0 , 7 , col);
  xTXT( (x+1) , 7, 3 , col);
  xTXT( (x+1) ,  4, 3 , col);
}
void lB (int x , int col) {
  lE(x , col); 
  yTXT ( (x+4) , 1 , 3 , col);
  yTXT ( (x+4), 5 , 2 , col);
}
void lC (int x, int col) {
  xTXT ( x , 0 , 4 , col);
  xTXT ( x , 7 , 4 , col);
  yTXT( x , 0 , 8 , col); 
}
void lD (int x, int col){
  lC(x,col);
  yTXT( (x+3), 1,6,col);
}
void lE (int x , int col) {
  lF(x, col);
  xTXT ( (x+1), 0 , 3 , col );
  }
void lF (int x , int col ) {
  yTXT ( x, 0 , 8, col);
  xTXT ( (x+1), 4 , 3 , col);
  xTXT ( (x+1), 7 , 3 , col);
}
void lG (int x, int col) {
  xTXT ( x , 0 , 4 , col);
  xTXT ( x , 7 , 4 , col);
  xTXT ( (x+2) , 4 , 2 , col);
  yTXT( x , 0 , 8 , col);  
  yTXT( (x+3) , 0 , 5 , col);
}
void lH (int x, int col) {
  xTXT ( (x+1) , 4, 2 , col);
  yTXT ( x , 0 , 8, col);
  yTXT ( (x+4) , 0 , 8, col);
}
void lI (int x, int col) {
  lT(x, col);
  xTXT (x , 0 ,5 , col);
}
void lJ (int x , int col) {
  xTXT ( x , 0 , 3 , col );
  yTXT ( (x+4) , 0 , 8 , col);
  xTXT ( (x+2) , 7 , 3, col);
}
void lK (int x , int col) { // TEST
  yTXT ( x, 0 ,8 ,col);
  diagDown( (x+1) , 3 , 3 , col);
  diagUp( (x+1) , 4 , 3 , col);
}
void lL (int x , int col) {
  xTXT ( x , 0 , 4 , col) ;
  yTXT ( x , 0 , 8 , col) ;
}
void lM (int x , int col) {
  yTXT ( x , 0 , 8 , col);
  yTXT ( (x+1) , 5 , 2 , col);
  yTXT ( (x+2) , 3 , 2 , col);
  yTXT ( (x+3) , 5 , 2 , col);
  yTXT ( (x+4) , 0 , 8 , col);
}
void lN (int x, int col) {
  yTXT ( x , 0 , 8 , col);
  yTXT ( (x+1) , 5 , 2 , col);
  yTXT ( (x+2) , 3 , 2 , col);
  yTXT ( (x+3) , 1 , 2 , col);
  yTXT ( (x+4) , 0 , 8 , col);
}
void lO (int x, int col) {
  xTXT ( (x+1) , 7 , 2 , col);
  xTXT ( (x+1) , 0 , 2 , col);
  yTXT ( (x) , 1 , 6 , col);
  yTXT ( (x+3) , 1 , 6 , col); 
}
void lP (int x , int col) {
  lF(x,col);
  yTXT( (x+4) , 5 , 2 , col);
}
void lQ (int x , int col) {
  lO(x,col);
  diagDown ( (x+2) , 1 , 2 , col);
}
void lR (int x , int col ) {
  lF(x , col);
  diagDown( x , 4 , 5 , col);
  yTXT( (x+4) , 5 , 2 , col);
}
void lS (int x , int col) {
  xTXT ( x , 0 , 4 , col);
  xTXT ( x , 4 , 4 , col);
  xTXT ( x , 7 , 4 , col);
  yTXT ( x , 5 , 2 , col);
  yTXT ( (x+3) , 1 , 3 , col); 
}
void lT (int x, int col) {
  xTXT ( x , 7 , 5 , col);
  yTXT ( (x+2) , 0 , 7 , col);
}
void lU (int x , int col){
  yTXT(x , 1 , 7, col);
  yTXT((x+3) , 1 , 7, col);
  xTXT((x+1) , 0 , 2, col);
}
void lV (int x , int col) {
  yTXT ( (x) , 6 , 3 , col);
  yTXT ( (x+1) , 3 , 3 , col);
  yTXT ( (x+2) , 0 , 2 , col);
  yTXT ( (x+3) , 3 , 3 , col);
  yTXT ( (x+4) , 6 , 3 , col);
}
void lW (int x , int col) {
  yTXT ( x , 0 , 8, col);
  yTXT ( (x+4) , 0 , 8, col);
  yTXT ( (x+1) , 1 , 2, col);
  yTXT ( (x+2) , 3 , 2, col);
  yTXT ( (x+3) , 1 , 2, col);
}
void lX (int x, int col) {
  yTXT ( x , 0 , 2 , col );
  yTXT ( x , 6 , 2 , col );
  yTXT ( ( x + 4 ) , 0 , 2 , col );
  yTXT ( ( x + 4 ) , 6 , 2 , col );
  yTXT ( ( x + 2 ) , 4 , 2 , col ); // the centre section
  xTXT ( ( x + 1  ) , 3 , 1 , col );
  xTXT ( ( x + 3  ) , 3 , 1 , col );
  xTXT ( ( x + 1  ) , 5 , 1 , col );
  xTXT ( ( x + 3  ) , 5 , 1 , col );  
}
void lY (int x , int col) {
  yTXT ( x , 5 , 3 , col);
  yTXT ( (x+3) , 2 , 6 , col);
  xTXT ( (x+1) , 4 , 2 , col);
  xTXT ( x , 0 , 2 , col);
  diagUp ( (x+1) , 0 , 3, col);
}
void lZ ( int x , int col) {
  xTXT ( x , 0 , 4 , col);
  xTXT ( x , 7 , 4 , col);
  yTXT ( x , 1 , 2 , col);
  yTXT ( (x+1) , 2 , 2 , col);
  yTXT ( (x+2) , 4 , 2 , col);
  yTXT ( (x+3) , 5 , 2 , col);
}
/*
void space (int x){
  for (int y = 0; y < kMatrixHeight; y++ ) leds [XY ( x , y ) ] += CRGB::Black;
}
*/
/************************************************************* Directional Drawing **************************************************************/
void xTXT (int x, int y , int len, int col) { //  x axis
  int lenVal;
  int correction;
  if( x < 0 & x > (x-len) ) { // if the x value is below zero but above the value of 0 - the length of character
     lenVal = len + x; // make a new length value with the sum of the length + the negative x value
     correction = len - lenVal; // use the length value against the original length to find the correction to x needed.
     for (int a = 0 ; a < lenVal ; a++) { // use new Length Value
       leds [ XY ( ( ( x + correction ) + a ) , y )] += cArray[col]; // use correction and new length to draw remaining pixels.
     }
   }
   else if ( x >= 0 ) { // if x value is greater than or equal to 0
     for (int a = 0 ; a < len ; a++) { // Use the original length
       leds [ XY ( ( x + a ) , y )] += cArray[col]; // Draw Character from allotted co-ordinate
     }
   }
 }
//No correction required on Y axis until scrolling up and down, any line on Y will be null.
void yTXT (int x, int y, int len , int col) { // y axis
  for (int a = 0 ; a < len ; a++) {
   leds [ XY  ( x , (y+a) )] += cArray[col];
  }
}
void diagUp (int x, int y, int len,  int col) { //diagonal up
  int lenVal;
  int correction;
  if( x < 0 & x > ( x - len ) ) { // if the x value is below zero but above the value of 0 - the length of character
     lenVal = len + x; // make a new length value with the sum of the length + the negative x value
     correction = len - lenVal; // use the length value against the original length to find the correction to x needed.
     for (int a = 0 ; a < lenVal ; a++) {
       leds [ XY ( ( ( x + correction ) + a ) , ( ( y + correction ) + a ) ) ] += cArray[col]; // add correction to both x and y as they are added to equally
     }
  }
  else if ( x >= 0 ) { // if x value is greater than or equal to 0
    for (int b = 0 ; b < len ; b++) {
       leds [ XY (  ( x + b ), ( y + b )  )] += cArray[col];
    }
  }
}
void diagDown(int x, int y, int len , int col) {
  int lenVal;
  int correction;
  if( x < 0 & x > ( x - len ) ) { // if the x value is below zero but above the value of 0 - the length of character
     lenVal = len + x; // make a new length value with the sum of the length + the negative x value
     correction = len - lenVal; // use the length value against the original length to find the correction to x needed.
     for (int a = 0 ; a < lenVal ; a++) {
       leds [ XY ( ( ( x + correction ) + a ) , ( ( y - correction ) - a ) )] += cArray[col];
     }
  }
  else if ( x >= 0 ) { // if x value is greater than or equal to 0
    for (int b = 0 ; b < len ; b++) {
      leds [ XY ( ( x + b ), ( y - b ) ) ] += cArray[col];
    }
  }
} 
/************************************************* FUNCTIONS ********************************************************/
void rainbow() {
  fill_rainbow( leds, 256, gHue, 7); // FastLED's built-in rainbow generator
}
void rainbowWithGlitter() {
  rainbow(); // built-in FastLED rainbow, plus some random sparkly glitter
  addGlitter(80); // Cool Glitter function (see Below)
}
void addGlitter( fract8 chanceOfGlitter) {
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}
void confetti() { // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}
void sinelon() { // a colored dot sweeping back and forth, with fading trails 
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}
void bpm() { // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}
void juggle() { // eight colored dots, weaving in and out of sync with each other 
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
void theLights() { //  speckles and strobes
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CRGB::White;
}
void buildUpCore(int col1, int col2, int col3) {
  int a = 0;
  int b = 1;
  int c = 2;
  while (a<NUM_LEDS) {
    if ((a % 3) == 0) {
     leds[a] += cArray[col1];     
    }
    if ((b % 3) == 1) {
     leds[b] += cArray[col2];   
    }
    if ((c % 3) == 2) {
     leds[c] += cArray[col3];   
    }
  a++;
  b++;
  c++;  
  }
}
void buildUp () {
  buildUpCore ( 0 , 1 , 2 ); // Colours from cArray.
  addGlitter(50);
}

/******************************************************************** STROBES ********************************************************/
void strobe () {
 fill_solid( leds, NUM_LEDS, CRGB::Black);
 const uint8_t kStrobeCycleLength = 4; // light every Nth frame
  static uint8_t sStrobePhase = 0;
  sStrobePhase = sStrobePhase + 1;
  if( sStrobePhase >= kStrobeCycleLength ) { 
    sStrobePhase = 0; 
  }
 // We only draw lit pixels when we're in strobe phase zero; 
  // in all the other phases we leave the LEDs all black.
  if( sStrobePhase == 0 ) {
    // The dash spacing cycles from 4 to 9 and back, 8x/min (about every 7.5 sec)
    uint8_t dashperiod= beatsin8( 8/*cycles per minute*/, 4,10);
    // The width of the dashes is a fraction of the dashperiod, with a minimum of one pixel
    uint8_t dashwidth = (dashperiod / 4) + 1;
    uint8_t zoomBPM = ZOOMING_BEATS_PER_MINUTE;
    int8_t  dashmotionspeed = beatsin8( (zoomBPM /2), 1,dashperiod);
    // This is where we reverse the direction under cover of high speed
    // visual aliasing.
    if( dashmotionspeed >= (dashperiod/2)) { 
      dashmotionspeed = 0 - (dashperiod - dashmotionspeed );
    }
    uint8_t cycle = beat8(2); // two cycles per minute
    uint8_t easedcycle = ease8InOutCubic( ease8InOutCubic( cycle));
    uint8_t wavecycle = cubicwave8( easedcycle);
    uint8_t hueShift = scale8( wavecycle,130);
    uint8_t strobesPerPosition = 1; // try 1..4
    discoWorker( dashperiod, dashwidth, dashmotionspeed, strobesPerPosition, hueShift);
  }
}

void discoWorker( 
    uint8_t dashperiod, uint8_t dashwidth, int8_t  dashmotionspeed,
    uint8_t stroberepeats,
    uint8_t huedelta)
 {
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
  const uint8_t kSaturation = 208;
  const uint8_t kValue = 255;
  drawRainbowDashes( sStartPosition, NUM_LEDS-1, // Change here for different strobes.
                     dashperiod, dashwidth, 
                     sStartHue, huedelta, 
                     kSaturation, kValue);
 }
static void drawRainbowDashes( 
  uint8_t startpos, uint16_t lastpos, uint8_t period, uint8_t width, 
  uint8_t huestart, uint8_t huedelta, uint8_t saturation, uint8_t value) 
  {
  uint8_t hue = huestart;
  for( uint16_t i = startpos; i <= lastpos; i += period) {
    CRGB color = CHSV( hue, saturation, value);  
    // draw one dash
    uint16_t pos = i;
    for( uint8_t w = 0; w < width; w++) {
      leds[ pos ] = color;
      pos++;
      if( pos >= NUM_LEDS) {
        break;
      }
    }
    hue += huedelta;
  }
}


void simpleStrobe () {
 fill_solid( leds, NUM_LEDS, CRGB::Black);
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
    uint8_t hueShift = 0; // NO SHIFT OF HUE IN COLOUR (we should rebuild in RGB...) 
   // uint8_t hueShift = scale8( wavecycle,130); // METHOD HOW HUE VALUE SHIFTS
    uint8_t strobesPerPosition = 2; // try 1..4
    strobeCore( dashperiod, dashwidth, dashmotionspeed, strobesPerPosition, hueShift);
  }
}
void strobeCore( 
    uint8_t dashperiod, uint8_t dashwidth, int8_t  dashmotionspeed, uint8_t stroberepeats,
    uint8_t huedelta) {
  static uint8_t sRepeatCounter = 0;
  static int8_t sStartPosition = 0;
  static uint8_t sStartHue = 10;
  //sStartHue += 1; //Shift the Colour little by little
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
  const uint8_t kSaturation = 0; // WHITE >> CURRENT COLOUR control (def 208)
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
      leds[ pos ] = color;
      pos++;
      if( pos >= NUM_LEDS) {
        break;
      }
    }
    hue += huedelta;
  }
}

/* TEST for RGB based Strobe - Unusual Behaviour atm, use HSV */ 
void basicStrobe () {
 fill_solid( leds, NUM_LEDS, CRGB::Black);
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
    CRGB color = CRGB::Blue; 
    uint16_t pos = i;
    for( uint8_t w = 0; w < width; w++) {
      leds[ pos ] = color;
      pos++;
      if( pos >= NUM_LEDS) {
        break;
      }
    }
  }
}

