#include "FastLED.h"
#include <Wire.h>
#define NUM_LEDS 58
#define DATA_PIN 2
#define DATA_PIN_2 3
#define DATA_PIN_3 4
#define DATA_PIN_4 5
#define FRAMES_PER_SECOND  120
CRGB leds[NUM_LEDS];
CRGB leds2[NUM_LEDS];
CRGB leds3[NUM_LEDS];
CRGB leds4[NUM_LEDS];
CRGB ourCol = CRGB(255, 255, 255);
CRGB startup[] = {CRGB(255, 123, 0), CRGB(0, 255, 45), CRGB(0, 123, 255), CRGB(0, 255, 255)};

int x = 0; // holder for i2c message
String string = "";
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
int timeSinceBt = 0;
int autoMode = 1;
int autoSecs = 10;
int connectDevs[] = {0,0,0,0}; 
void setup() {
  Wire.begin(8);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
  Serial.println("Ready for IC2");
  Serial.println("READING ANALOG INPUTS");
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN_2, GRB>(leds2, NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN_3, GRB>(leds3, NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN_4, GRB>(leds4, NUM_LEDS);
  FastLED.setBrightness(128);
  int i = 0;
  while (i < 4) {
    ourCol = startup[i];
    turnOn();
    FastLED.show();
    FastLED.delay(300);
    i++;
  }

}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void requestEvent() {
  String msg = "STR,"+String(ourCol.r)+","+String(ourCol.g)+","+String(ourCol.b)+","+String(x)+","+String(autoMode)+","+String(autoSecs);
  char bufer[50];
  msg.toCharArray(bufer, 50);
  Serial.println(msg);
  Wire.write(bufer);
}
void receiveEvent(int howMany) {
  while (Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    string.concat(c);       // print the character
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
    x = ss.toInt();
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
    x = random(2, 9);
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

  }
  //call function
  copyLeds();

  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND); // insert a delay to keep the framerate modest

  //  EVERY_N_MILLISECONDS( 20 )
}
void copyLeds() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds2[i] = leds[i];
    leds3[i] = leds[i];
    leds4[i] = leds[i];
  }
}
void turnOff() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB( 0, 0, 0);
  }
}
void turnOn() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ourCol;
  }
}
void theLights() { //  speckles and strobes
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] = ourCol;
}
void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}
void addGlitter( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
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
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] = ourCol;
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS - 1 );
  leds[pos] = ourCol;
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ourCol;
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for ( int i = 0; i < 8; i++) {
    leds[beatsin16( i + 7, 0, NUM_LEDS - 1 )] |= ourCol;
    leds[beatsin16( i + 7, 0, NUM_LEDS - 1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
