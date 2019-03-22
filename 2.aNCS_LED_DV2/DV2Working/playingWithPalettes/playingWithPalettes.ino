
#include <FastLED.h>
#define NUM_STRIPS 3 // defines the number of strips n use. these 3 lines will need additions to the parser to make fully modular.
#define NUM_LEDS_PER_STRIP 29 //defines number of LED's per strip, see above.
#define NUM_LEDS (NUM_LEDS_PER_STRIP * NUM_STRIPS) //calculates the total number of LED's based on the above 2 values.
#define BRIGHTNESS  64
CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 100
#define ZOOMING_BEATS_PER_MINUTE 122
#define STROBE_BEATS_PER_MINUTE 97.5
#define FL(aa,bb) for (int i = aa; i < bb; i++) //for loop definition.
CRGB ourCol = CRGB(255, 255, 255);
CRGBPalette16 currentPalette;
TBlendType    currentBlending;
uint8_t brightness = 255;
uint8_t patternNo = 0;
// Gradient palette "Pastel1_08_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/cb/qual/tn/Pastel1_08.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 64 bytes of program space.

DEFINE_GRADIENT_PALETTE( Pastel1_08_gp ) {//group 1
  0, 244, 118, 98,
  31, 244, 118, 98,
  31, 101, 157, 190,
  63, 101, 157, 190,
  63, 142, 213, 133,
  95, 142, 213, 133,
  95, 177, 154, 192,
  127, 177, 154, 192,
  127, 252, 178, 87,
  159, 252, 178, 87,
  159, 255, 255, 145,
  191, 255, 255, 145,
  191, 192, 176, 120,
  223, 192, 176, 120,
  223, 249, 180, 210,
  255, 249, 180, 210
};
// Gradient palette "Paired_08_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/cb/qual/tn/Paired_08.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 64 bytes of program space.

DEFINE_GRADIENT_PALETTE( Paired_08_gp ) {//group 1
  0,  83, 159, 190,
  31,  83, 159, 190,
  31,   1, 48, 106,
  63,   1, 48, 106,
  63, 100, 189, 54,
  95, 100, 189, 54,
  95,   3, 91,  3,
  127,   3, 91,  3,
  127, 244, 84, 71,
  159, 244, 84, 71,
  159, 188,  1,  1,
  191, 188,  1,  1,
  191, 249, 135, 31,
  223, 249, 135, 31,
  223, 255, 55,  0,
  255, 255, 55,  0
};
// Gradient palette "Dark2_08_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/cb/qual/tn/Dark2_08.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 64 bytes of program space.

DEFINE_GRADIENT_PALETTE( Dark2_08_gp ) {//group 1
  0,   1, 88, 37,
  31,   1, 88, 37,
  31, 167, 29,  1,
  63, 167, 29,  1,
  63,  33, 41, 105,
  95,  33, 41, 105,
  95, 197,  4, 54,
  127, 197,  4, 54,
  127,  23, 99,  1,
  159,  23, 99,  1,
  159, 194, 105,  1,
  191, 194, 105,  1,
  191,  83, 46,  1,
  223,  83, 46,  1,
  223,  23, 33, 25,
  255,  23, 33, 25
};
// Gradient palette "RdYlGn_08_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/cb/div/tn/RdYlGn_08.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 64 bytes of program space.

DEFINE_GRADIENT_PALETTE( RdYlGn_08_gp ) {//group 2
  0, 163,  6,  2,
  31, 163,  6,  2,
  31, 227, 39,  9,
  63, 227, 39,  9,
  63, 249, 109, 22,
  95, 249, 109, 22,
  95, 252, 191, 55,
  127, 252, 191, 55,
  127, 167, 221, 55,
  159, 167, 221, 55,
  159,  83, 178, 28,
  191,  83, 178, 28,
  191,  23, 131, 23,
  223,  23, 131, 23,
  223,   1, 81, 14,
  255,   1, 81, 14
};
// Gradient palette "RdYlBu_08_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/cb/div/tn/RdYlBu_08.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 64 bytes of program space.

DEFINE_GRADIENT_PALETTE( RdYlBu_08_gp ) {//group 2
  0, 163,  6,  2,
  31, 163,  6,  2,
  31, 227, 39,  9,
  63, 227, 39,  9,
  63, 249, 109, 22,
  95, 249, 109, 22,
  95, 252, 191, 61,
  127, 252, 191, 61,
  127, 182, 229, 237,
  159, 182, 229, 237,
  159,  90, 178, 203,
  191,  90, 178, 203,
  191,  32, 108, 155,
  223,  32, 108, 155,
  223,   8, 45, 106,
  255,   8, 45, 106
};
// Gradient palette "RdBu_08_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/cb/div/tn/RdBu_08.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 64 bytes of program space.

DEFINE_GRADIENT_PALETTE( RdBu_08_gp ) {//group 2
  0, 100,  1,  2,
  31, 100,  1,  2,
  31, 161, 29, 12,
  63, 161, 29, 12,
  63, 227, 97, 47,
  95, 227, 97, 47,
  95, 249, 182, 137,
  127, 249, 182, 137,
  127, 152, 201, 219,
  159, 152, 201, 219,
  159,  59, 144, 180,
  191,  59, 144, 180,
  191,   7, 75, 130,
  223,   7, 75, 130,
  223,   1, 33, 95,
  255,   1, 33, 95
};
// Gradient palette "PuOr_08_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/cb/div/tn/PuOr_08.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 64 bytes of program space.

DEFINE_GRADIENT_PALETTE( PuOr_08_gp ) {//group 2
  0, 101, 24,  1,
  31, 101, 24,  1,
  31, 182, 57,  1,
  63, 182, 57,  1,
  63, 249, 124, 23,
  95, 249, 124, 23,
  95, 252, 191, 109,
  127, 252, 191, 109,
  127, 165, 180, 207,
  159, 165, 180, 207,
  159, 100, 105, 156,
  191, 100, 105, 156,
  191,  42, 44, 95,
  223,  42, 44, 95,
  223,  14,  4, 52,
  255,  14,  4, 52
};
// Gradient palette "PRGn_08_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/cb/div/tn/PRGn_08.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 64 bytes of program space.

DEFINE_GRADIENT_PALETTE( PRGn_08_gp ) {//group 2
  0,  34,  4, 48,
  31,  34,  4, 48,
  31,  67, 41, 93,
  63,  67, 41, 93,
  63, 125, 97, 151,
  95, 125, 97, 151,
  95, 197, 169, 201,
  127, 197, 169, 201,
  127, 167, 223, 158,
  159, 167, 223, 158,
  159,  83, 182, 79,
  191,  83, 182, 79,
  191,  17, 109, 22,
  223,  17, 109, 22,
  223,   1, 48,  5,
  255,   1, 48,  5
};
// Gradient palette "Fuschia_8_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/ds/fuschia/tn/Fuschia-8.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 12 bytes of program space.

DEFINE_GRADIENT_PALETTE( Fuschia_8_gp ) {//group 3  purple
  0,  44,  4, 214,
  127, 123,  1, 233,
  255, 255,  0, 255
};
// Gradient palette "saga_07_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/saga/tn/saga-07.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 80 bytes of program space.

DEFINE_GRADIENT_PALETTE( saga_07_gp ) {//group 3 red
  0, 255, 255, 255,
  13, 255, 225, 221,
  26, 255, 199, 192,
  40, 255, 173, 164,
  53, 255, 151, 140,
  67, 255, 128, 117,
  80, 255, 109, 98,
  93, 255, 92, 80,
  107, 255, 75, 64,
  120, 255, 61, 51,
  134, 255, 48, 38,
  147, 255, 37, 29,
  161, 255, 27, 20,
  174, 255, 19, 14,
  187, 255, 13,  9,
  201, 255,  8,  5,
  214, 255,  4,  2,
  228, 255,  1,  1,
  241, 255,  1,  1,
  255, 255,  0,  0
};
// Gradient palette "saga_08_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/saga/tn/saga-08.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 80 bytes of program space.

DEFINE_GRADIENT_PALETTE( saga_08_gp ) {//group 3 green
  0, 255, 255, 255,
  13, 220, 239, 221,
  26, 190, 225, 192,
  40, 161, 211, 164,
  53, 137, 199, 140,
  67, 113, 186, 117,
  80,  94, 173, 98,
  93,  77, 161, 80,
  107,  60, 151, 64,
  120,  47, 139, 51,
  134,  35, 128, 38,
  147,  26, 118, 29,
  161,  18, 109, 20,
  174,  12, 100, 14,
  187,   7, 91,  9,
  201,   4, 82,  5,
  214,   2, 75,  2,
  228,   1, 68,  1,
  241,   1, 60,  1,
  255,   0, 55,  0
};
// Gradient palette "saga_09_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/saga/tn/saga-09.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 80 bytes of program space.

DEFINE_GRADIENT_PALETTE( saga_09_gp ) {//group 3 blue
  0, 255, 255, 255,
  13, 220, 225, 245,
  26, 190, 199, 237,
  40, 161, 173, 228,
  53, 137, 151, 221,
  67, 113, 128, 214,
  80,  94, 109, 205,
  93,  77, 92, 199,
  107,  60, 75, 192,
  120,  47, 61, 184,
  134,  35, 48, 178,
  147,  26, 37, 170,
  161,  18, 27, 164,
  174,  12, 19, 158,
  187,   7, 13, 151,
  201,   4,  8, 145,
  214,   2,  4, 140,
  228,   1,  1, 133,
  241,   1,  1, 128,
  255,   0,  0, 123
};
// Gradient palette "saga_10_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/saga/tn/saga-10.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 80 bytes of program space.

DEFINE_GRADIENT_PALETTE( saga_10_gp ) {//group 4 yellow->red
  0, 255, 255,  0,
  13, 244, 225,  0,
  26, 237, 199,  0,
  40, 227, 173,  0,
  53, 220, 151,  0,
  67, 213, 128,  0,
  80, 203, 109,  0,
  93, 197, 92,  0,
  107, 190, 75,  0,
  120, 182, 61,  0,
  134, 175, 48,  0,
  147, 167, 37,  0,
  161, 161, 27,  0,
  174, 155, 19,  0,
  187, 148, 13,  0,
  201, 142,  8,  0,
  214, 137,  4,  0,
  228, 130,  1,  0,
  241, 125,  1,  0,
  255, 120,  0,  0
};
// Gradient palette "saga_11_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/saga/tn/saga-11.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 80 bytes of program space.

DEFINE_GRADIENT_PALETTE( saga_11_gp ) {//group 4 yellow->green
  0, 255, 255,  0,
  13, 220, 231,  0,
  26, 190, 211,  0,
  40, 161, 191,  0,
  53, 137, 173,  0,
  67, 113, 156,  0,
  80,  94, 139,  0,
  93,  77, 124,  0,
  107,  60, 109,  0,
  120,  47, 96,  0,
  134,  35, 82,  0,
  147,  26, 71,  0,
  161,  18, 60,  0,
  174,  12, 51,  0,
  187,   7, 42,  0,
  201,   4, 34,  0,
  214,   2, 27,  0,
  228,   1, 21,  0,
  241,   1, 16,  0,
  255,   0, 11,  0
};
// Gradient palette "saga_12_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/saga/tn/saga-12.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 80 bytes of program space.

DEFINE_GRADIENT_PALETTE( saga_12_gp ) {//group 4 yellow -> blue.
  0, 255, 255,  0,
  13, 220, 225,  1,
  26, 190, 199,  1,
  40, 161, 173,  2,
  53, 137, 151,  5,
  67, 113, 128,  9,
  80,  94, 109, 14,
  93,  77, 92, 20,
  107,  60, 75, 29,
  120,  47, 61, 38,
  134,  35, 48, 51,
  147,  26, 37, 64,
  161,  18, 27, 80,
  174,  12, 19, 98,
  187,   7, 13, 117,
  201,   4,  8, 140,
  214,   2,  4, 164,
  228,   1,  1, 192,
  241,   1,  1, 221,
  255,   0,  0, 255
};
// Gradient palette "saga_13_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/saga/tn/saga-13.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 80 bytes of program space.

DEFINE_GRADIENT_PALETTE( saga_13_gp ) {//group 4 green -> red
  0,   0, 255,  0,
  13,   1, 227,  0,
  26,  10, 201,  0,
  40,  31, 176,  0,
  53,  66, 154,  0,
  67, 120, 135,  0,
  80, 140, 115,  0,
  93, 165, 97,  0,
  107, 192, 81,  0,
  120, 222, 67,  0,
  134, 255, 55,  0,
  147, 237, 43,  0,
  161, 222, 33,  0,
  174, 206, 24,  0,
  187, 192, 17,  0,
  201, 179, 11,  0,
  214, 120,  7,  0,
  228,  74,  3,  0,
  241,  41,  1,  0,
  255,  19,  1,  0
};
// Gradient palette "saga_14_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/saga/tn/saga-14.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 80 bytes of program space.
DEFINE_GRADIENT_PALETTE( saga_14_gp ) {//group 4 blue -> red.
  0,   0,  0, 255,
  13,   1,  0, 221,
  26,   1,  0, 192,
  40,   2,  0, 164,
  53,   4,  0, 140,
  67,   7,  0, 117,
  80,  12,  0, 98,
  93,  18,  0, 80,
  107,  26,  0, 64,
  120,  35,  0, 51,
  134,  47,  0, 38,
  147,  60,  0, 29,
  161,  77,  0, 20,
  174,  94,  0, 14,
  187, 113,  0,  9,
  201, 137,  0,  5,
  214, 161,  0,  2,
  228, 190,  0,  1,
  241, 220,  0,  1,
  255, 255,  0,  0
};
// Gradient palette "saga_15_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/saga/tn/saga-15.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 80 bytes of program space.

DEFINE_GRADIENT_PALETTE( saga_15_gp ) {//group 4 blue -> green.
  0,   0,  0, 255,
  13,   0,  1, 221,
  26,   0,  1, 192,
  40,   0,  4, 164,
  53,   0,  8, 140,
  67,   0, 13, 117,
  80,   0, 19, 98,
  93,   0, 27, 80,
  107,   0, 37, 64,
  120,   0, 48, 51,
  134,   0, 61, 38,
  147,   0, 75, 29,
  161,   0, 92, 20,
  174,   0, 109, 14,
  187,   0, 128,  9,
  201,   0, 151,  5,
  214,   0, 173,  2,
  228,   0, 199,  1,
  241,   0, 225,  1,
  255,   0, 255,  0
};
// Gradient palette "saga_17_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/saga/tn/saga-17.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 80 bytes of program space.

DEFINE_GRADIENT_PALETTE( saga_17_gp ) {//group green -> red.
  0,   0, 81,  1,
  13,   0, 115,  4,
  26,   0, 154, 12,
  40,   0, 201, 25,
  53,   0, 255, 44,
  67,   2, 231, 57,
  80,  12, 209, 74,
  93,  35, 187, 92,
  107,  75, 168, 114,
  120, 135, 149, 138,
  134, 155, 125, 79,
  147, 177, 104, 38,
  161, 201, 86, 14,
  174, 227, 69,  2,
  187, 255, 55,  0,
  201, 192, 33,  0,
  214, 140, 17,  0,
  228, 100,  7,  0,
  241,  66,  1,  0,
  255,  41,  0,  0
};


void setup() {
  Serial.begin(9600);
  Serial.println("setup begin");
  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<WS2812B, 2, GRB>(leds, 0, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, 3, GRB>(leds, NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, 4, GRB>(leds, 2 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.setBrightness(  BRIGHTNESS );
  currentBlending = LINEARBLEND;
  Serial.println("setup complete");
}

void loop()
{
  static uint8_t numberOfPalettes = 18;
  static uint8_t paletteIndex = 18;
  static uint8_t startIndex = 0;
  static uint8_t numberOfPatterns = 4;
  startIndex = startIndex + 1; /* motion speed */
   EVERY_N_SECONDS(5) {
     paletteIndex++;
     Serial.println("palette incresed ");
     if (paletteIndex > numberOfPalettes) {
       paletteIndex = 0;
       Serial.print(paletteIndex);
     }
    }
  EVERY_N_SECONDS(15) {
    patternNo++;
    Serial.println("pattern increased");
    if (patternNo > numberOfPatterns) {
      patternNo = 0;
      Serial.print(patternNo);
    }
  }
  switch (paletteIndex) {
    case 0:
      currentPalette = Pastel1_08_gp;
      break;
    case 1:
      currentPalette = Paired_08_gp;
      break;
    case 2:
      currentPalette = Dark2_08_gp;
      break;
    case 3:
      currentPalette = RdYlGn_08_gp;
      break;
    case 4:
      currentPalette = RdYlBu_08_gp;
      break;
    case 5:
      currentPalette = RdBu_08_gp;
      break;
    case 6 :
      currentPalette = PuOr_08_gp;
      break;
    case 7:
      currentPalette = PRGn_08_gp;
      break;
    case 8:
      currentPalette = Fuschia_8_gp;
      break;
    case 9:
      currentPalette = saga_07_gp;
      break;
    case 10:
      currentPalette = saga_08_gp;
      break;
    case 11:
      currentPalette = saga_09_gp;
      break;
    case 12:
      currentPalette = saga_10_gp;
      break;
    case 13:
      currentPalette = saga_11_gp;
      break;
    case 14:
      currentPalette = saga_12_gp;
      break;
    case 15:
      currentPalette = saga_13_gp;
      break;
    case 16:
      currentPalette = saga_14_gp;
      break;
    case 17:
      currentPalette = saga_15_gp;
      break;
    case 18:
      currentPalette = saga_17_gp;
      break;
  }
  switch (patternNo) {
    case 0:
      theLights(startIndex);
      break;
    case 1:
      juggle(startIndex);
      break;
    case 2:
      bpm(startIndex);
      break;
    case 3:
      sinelon(startIndex);
      break;
    case 4:
      confetti(startIndex);
      break;
  }

  // FillLEDsFromPaletteColors( startIndex);
  //theLights(startIndex);
  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void FillLEDsFromPaletteColors( uint8_t colorIndex) {
  for ( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}
void theLights(uint8_t colorIndex) { //  speckles and strobes
  fadeToBlackBy(&(leds[0]), NUM_LEDS, 10);
  int pos = random16(0, NUM_LEDS);
  leds[pos] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
}
void juggle(uint8_t colorIndex) {// eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(&(leds[0]), NUM_LEDS, 20);
  byte dothue = 0;
  ourCol = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
  FL(0, 8) {
    leds[beatsin16( i + 7, 0, NUM_LEDS - 1 )] |= ourCol;
    leds[beatsin16( i + 7, 0, NUM_LEDS - 1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
void bpm(uint8_t colorIndex) {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  FL(0, NUM_LEDS) { //9948
    leds[i] = ColorFromPalette(currentPalette, colorIndex + (i * 2), beat - colorIndex + (i * 10));
  }
}
void sinelon(uint8_t colorIndex) {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(&(leds[0]), NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS - 1 ) ;
  leds[pos] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
}
void confetti(uint8_t colorIndex) {
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(&(leds[0]), NUM_LEDS, 10);
  int pos = random16(0, NUM_LEDS);
  leds[pos] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
}
