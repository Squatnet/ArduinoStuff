#include <PJON.h>
#include "FastLED.h"
#define fL(x,aa,bb) for (int x = aa; x < bb; x++)

#define NUM_LEDS 29
#define DATA_PIN 2
#define DATA_PIN_2 3
#define DATA_PIN_3 4
#define DATA_PIN_4 5

#define FRAMES_PER_SECOND  120
#define ZOOMING_BEATS_PER_MINUTE 122
#define STROBE_BEATS_PER_MINUTE 97.5
#define CONNECTED_STRIPS 4

CRGB ledsA[NUM_LEDS];
CRGB ledsB[NUM_LEDS];
CRGB ledsC[NUM_LEDS];
CRGB ledsD[NUM_LEDS];
CRGB ourCol = CRGB(255, 255, 255);
CRGB startup[] = {CRGB(255, 123, 0), CRGB(0, 255, 45), CRGB(0, 123, 255), CRGB(0, 255, 255)};

uint8_t bus_id[] = {0, 0, 0, 0};
PJON<SoftwareBitBang> bus(bus_id, 2);

int x = 0; // holder for pattern message
String string = "";
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
int timeSinceBt = 0;
int autoMode = 0;
int autoSecs = 10;

void setup() {
  Serial.begin(9600);
  Serial.println("Ready for i2c");
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(ledsA, NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN_2, GRB>(ledsB, NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN_3, GRB>(ledsC, NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN_4, GRB>(ledsD, NUM_LEDS);
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
}


void receiver_handler(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  const char * arr = payload;
  Serial.println(arr);
  timeSinceBt = 0;
  String str = arr;
  Serial.println(str);
  str.trim();
  String strB = string.substring(0, 1);
  str.remove(0, 1);
  if (strB == "X") {
    autoMode = str.toInt(); // set autoMode IO
    Serial.print("AUTO");
    Serial.print(autoMode);
  }
  else if (strB == "S") {
    autoSecs = str.toInt(); // set seconds value
    Serial.print("AUTOSECS = ");
    Serial.println(autoSecs);
  }
  else {
    x = strB.toInt(); // pattern choice 
    if (str.length() == 6) {
      char charbuf[8];
      str.toCharArray(charbuf, 8);
      long int rgb = strtol(charbuf, 0, 16); //=>rgb=0x001234FE;
      byte r = (byte)(rgb >> 16);
      byte g = (byte)(rgb >> 8);
      byte b = (byte)(rgb);
      ourCol = CRGB(r, g, b); // set the colour value!
    }
  }
  str = "";
}
  
void randX() {
  if (autoMode == 1) {
    x = random(2, 10);
    Serial.print("RANDOM ");
    Serial.println(x);
  }
}
void loop() {
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
void copyLeds() {
  ///for (int i = 0; i < NUM_LEDS; i++) 
  fL(i,0,NUM_LEDS){
    ledsB[i] = ledsA[i];
    ledsC[i] = ledsA[i];
    ledsD[i] = ledsA[i];
  }
}
void turnOff() {
  fL(i,0,NUM_LEDS) {
    ledsA[i] = CRGB( 0, 0, 0);
  }
}
void turnOn() {
  fL(i,0,NUM_LEDS) {
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
  fL(i,0,NUM_LEDS) { //9948
    ledsA[i] = ourCol;
    ledsA[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( ledsA, NUM_LEDS, 20);
  byte dothue = 0;
  fL(i,0,8) {
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
