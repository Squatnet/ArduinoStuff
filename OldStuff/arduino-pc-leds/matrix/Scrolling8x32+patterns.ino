#include <FastLED.h>

#include <LEDMatrix.h>
#include <LEDText.h>
#include <FontANCS.h>

#define LED_PIN        6
#define COLOR_ORDER    GRB
#define CHIPSET        WS2812B
#define MATRIX_WIDTH   32
#define MATRIX_HEIGHT  8
#define NUM_LEDS       256
#define MATRIX_TYPE    HORIZONTAL_ZIGZAG_MATRIX
#define FRAMES_PER_SECOND  120

cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;
cLEDText ScrollingMsg;
CRGB tst[256];
const unsigned char TxtDemo[] = {  EFFECT_FRAME_RATE "\x04" EFFECT_HSV_AV "\xff\xff\xff\xff\x00\xff" EFFECT_SCROLL_LEFT "             AUTONOISECREATIONSTATION NOISILY APPLICATION 2018            " 
                                   EFFECT_FRAME_RATE "\x04" EFFECT_HSV_AV "\x00\x00\xff\x40\x00\x00" EFFECT_SCROLL_LEFT "             AUTONOISECREATIONSTATION NOISILY APPLICATION 2018            "
                                   EFFECT_FRAME_RATE "\x04" EFFECT_HSV_AV "\x00\xff\x00\xff\x00\xff" EFFECT_SCROLL_LEFT "             AUTONOISECREATIONSTATION NOISILY APPLICATION 2018            " 

};

void setup()
{
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds[0], leds.Size());
  FastLED.setBrightness(64);
  FastLED.clear(true);
  delay(500);
  FastLED.showColor(CRGB::Red);
  delay(1000);
  FastLED.showColor(CRGB::Lime);
  delay(1000);
  FastLED.showColor(CRGB::Blue);
  delay(1000);
  FastLED.showColor(CRGB::White);
  delay(1000);
  FastLED.show();

  ScrollingMsg.SetFont(aNCSFontData); // or Robotron.
  ScrollingMsg.Init(&leds, leds.Width(), ScrollingMsg.FontHeight() + 1, 0, 0);
  ScrollingMsg.SetText((unsigned char *)TxtDemo, sizeof(TxtDemo) - 1);
  ScrollingMsg.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 0xff, 0x00, 0xff);
}

typedef void (*PatternList[])();
PatternList gPatterns = {  theLights, scroll, rainbow, rainbowWithGlitter, scroll, confetti, sinelon, bpm} ;

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
void scroll()
{
  if (ScrollingMsg.UpdateText() == -1) // if end of text
    ScrollingMsg.SetText((unsigned char *)TxtDemo, sizeof(TxtDemo) - 1); //reset text
  else
    return;//exist. Library *should* update and iterate steps automatically.
    }
void testWank() {
  for (int i=0;i<256;i++){
    leds(i) = tst[i];
      
   }
}
void theLights() { //  speckles and strobes
  fadeToBlackBy( tst, 256, 10);
  int pos = random16(256);
  tst[pos] += CRGB::White;
  testWank();
}
void rainbow() {
  fill_rainbow( tst, NUM_LEDS, gHue, 7); // FastLED's built-in rainbow generator
  testWank();
}

void rainbowWithGlitter() {
  rainbow(); // built-in FastLED rainbow, plus some random sparkly glitter
  addGlitter(80); // Cool Glitter function (see Below)
  testWank();
}
void addGlitter( fract8 chanceOfGlitter) {
  if( random8() < chanceOfGlitter) {
    tst[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() { // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( tst, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  tst[pos] += CHSV( gHue + random8(64), 200, 255);
  testWank();
}

void sinelon() { // a colored dot sweeping back and forth, with fading trails 
  fadeToBlackBy( tst, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  tst[pos] += CHSV( gHue, 255, 192);
  testWank();
}

void bpm() { // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 97;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    tst[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
  testWank();
}
