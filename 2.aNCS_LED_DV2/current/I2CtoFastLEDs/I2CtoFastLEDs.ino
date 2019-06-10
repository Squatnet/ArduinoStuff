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
#define FRAMES_PER_SECOND  40
#define ZOOMING_BEATS_PER_MINUTE 200
#define STROBE_BEATS_PER_MINUTE 300
#define NUM_STRIPS 4 // defines the number of strips n use. these 3 lines will need additions to the parser to make fully modular.
#define NUM_LEDS_PER_STRIP 28 //defines number of LED's per strip, see above.
#define NUM_LEDS NUM_LEDS_PER_STRIP * NUM_STRIPS //calculates the total number of LED's based on the above 2 values.
#define I2C_ADDR 4
#define BRIGHTNESS 64 // Used only for fastLeds set_brightness

#define FL(aa,bb) for (int i = aa; i < bb; i++) //for loop definition.

CRGB leds[NUM_LEDS]; //makes an array of CRGB values, this allows us to address each LED individualy or as a group.
CRGB ourCol = CRGB(255, 255, 255); //used to specify an individual color for use in patterns.
CRGB startup[] = {CRGB(255, 123, 0), CRGB(0, 255, 45), CRGB(0, 123, 255), CRGB(0, 255, 255)}; //used in setup to flash 3 colors ??
CRGBPalette16 currentPalette;//holds the palette
TBlendType currentBlending;//blending type
String string = ""; //holder for the parser string.
int patternNumber = 0; // holder for i2c message which sets pattern when we address the stips as one array.
int y = 0;//holder for i2c message which sets pattern when we adressing strips individually.
int timeSinceBt = 0; //legacy currently unused. set to 0 when message comes in, then increments each second (time since last message recieved.)??
int autoMode = 1;//if 1 increments the pattern and palette. if 2 only increments palette.
int autoSecs = 4;//sets the upper bound for timeSinceBt function.
int stripNumber = 1;//stores the strip that we wish to set the pattern on.
int individualStripMode = 0;//holds wether we are addressing all the stips(0)or individual strips (1)
int paletteMode = 1;//holds if we sending indivdual colors to the patterns or a palette array.
int paletteNumber = 0;//holds the number for which palette is in use when paletteMode is on.
int numberOfPalettes = 18; //total number of palettes available -1.
int colorIndex = 0;//holds the position in the palette array for the color to show.
int LEDStart = 0;//this holds the number of the first LED in the arry to start printing a pattern to.
int LEDEnd = 0;//this holds the number of the last LED in the arry to start printing a pattern to.
int NoLEDs = 0;//this holds how many LED's we need to address. (may be a better way of doin this ToDo)
int patternStore[NUM_STRIPS + 1]; //this array holds the pattern number for each strip.
int delayStore[NUM_STRIPS + 1];
int standardDelay = 20;//initial delay that the maths of the variable below relates to.
long variableDelay = 128; //0..256 visible to parser and used to set FastLED.delay values.
int currentDelay = 128; // The current delay of the pattern
uint8_t paletteBrightness = 64; // Set to 64 to save power for dev.

bool debugLED = false;//when true will flash the onboard LED each time the loop compleates.
bool rainbowWithGlitterTrigger = 0;//a trigger to tell if rainbow with glitter is being called, so as to not have multiple delays.

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
DEFINE_GRADIENT_PALETTE( Fuschia_8_gp ) {//group 3  purple
  0,  44,  4, 214,
  127, 123,  1, 233,
  255, 255,  0, 255
};
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

void(* resetFunc) (void) = 0; // Software reset hack

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) { //if a message is coming in over 12c, this concatinates it into a string and passes the string to the parser.
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
void setLEDs() {//this function sets the boundares for LED addressing.
  if (individualStripMode == 1) {
    NoLEDs = NUM_LEDS_PER_STRIP;//if we are adressing individual strips  the number of LEDs to change will be equal to the number of LEDs in one strip.
    FL(1, NUM_STRIPS + 1) { //loops through each strip
      if ((individualStripMode == 1) && (stripNumber == i)) {//if it is equal to the strip we wish to address, set the start and end number of the strip.
        LEDEnd = (NUM_LEDS_PER_STRIP * i);
        LEDStart = ((NUM_LEDS_PER_STRIP * i) - (NUM_LEDS_PER_STRIP));
      }
    }
  }
  if (individualStripMode == 0) {//if addressing all strips as one.
    NoLEDs = NUM_LEDS;//number of LEDs is equal to the total number of LEDs
    LEDStart = 0;//set start and end possition in array
    LEDEnd = NUM_LEDS;
  }
}
void parser() {
  DPRINTLN("PARSER");
  byte randTrigger1 = 0;
  byte randTrigger = 0; //initializes a randPattern trigger (didn't need to be global);
  while (string.length() >= 1) { // While there message left to read.
    DPRINT("Message is ");
    DPRINT(string);
    DPRINT(" With length ");
    DPRINTLN(string.length());
    if (!string.endsWith(","))string.concat(","); // STOP CRASHING;
    String subs = string.substring(0, (string.indexOf(','))); // get everything until the first comma.
    DPRINT("SubStr - ");
    DPRINT(subs);
    string.remove(0, (string.indexOf(",") + 1)); // remove everything up to and including the first comma
    DPRINT(" String - ");
    DPRINTLN(string);
    if (subs.startsWith("Rst"))resetFunc();
    if (subs.startsWith("Atm")) { // next value is boolean for automode
      DPRINT("ATM ");
      String atm = string.substring(0, (string.indexOf(","))); // get until first comma
      autoMode = atm.toInt(); // also an integer
      DPRINT(atm);
      DPRINT(" - ");
      DPRINTLN(autoMode);
      string.remove(0, (string.indexOf(",") + 1)); // remove it
    }
    if (subs.startsWith("Ats")) { // next value is time between automode calls.
      DPRINT("ATS ");
      String ats = string.substring(0, (string.indexOf(","))); // get the value,
      autoSecs = ats.toInt();
      string.remove(0, (string.indexOf(",") + 1)); // remove the value and trailing comma
    }
    if (subs.startsWith("ISt")) { // individual Strip Mode. next value if 0, addresses all strips, if 1 addresses strips individually.
      DPRINT("individual Strip Mode ");
      String ist = string.substring(0, (string.indexOf(","))); // get everything until the comma
      DPRINT(ist);
      DPRINT(" - ");
      individualStripMode = ist.toInt(); // Its going to be an integer.
      DPRINTLN(individualStripMode);
      string.remove(0, (string.indexOf(",") + 1)); // Remove the value
      if (individualStripMode == 0) {
        setLEDs();// if 0 set the LED bounds, (if 1 this is set in Ptn bellow)
      }
    }
    if (subs.startsWith("SNo")) { // next value is strip No. to address.
      DPRINT("Strip No. ");
      String sno = string.substring(0, (string.indexOf(","))); // get everything until the comma
      DPRINT(sno);
      DPRINT(" - ");
      stripNumber = sno.toInt();
      DPRINTLN(stripNumber);
      string.remove(0, (string.indexOf(",") + 1)); // Remove the value
      setLEDs();
    }
    if (subs.startsWith("Ptn")) { // next value is number of pattern.
      DPRINT("PTN ");
      String ptn = string.substring(0, (string.indexOf(","))); // get everything until the comma
      DPRINT(ptn);
      DPRINT(" - ");
      patternNumber = ptn.toInt();
      DPRINTLN(patternNumber);
      string.remove(0, (string.indexOf(",") + 1)); // Remove the value
      if (individualStripMode == 1) {
        setLEDs();
        patternStore[stripNumber] = patternNumber;
      }
      DPRINT("pattern set to");
      DPRINTLN(patternNumber);
    }
    if (subs.startsWith("Col")) { // its the color
      String r = string.substring(0, (string.indexOf(","))); // first bit is red,
      ourCol.r = r.toInt(); // convert to an int
      string.remove(0, (string.indexOf(",") + 1)); // remove red and comma
      String b = string.substring(0, (string.indexOf(","))); // next up its blue
      ourCol.b = b.toInt(); // to integer
      string.remove(0, (string.indexOf(",") + 1)); // remove blue and comma
      String g = string.substring(0, (string.indexOf(","))); // then green
      ourCol.g = g.toInt(); // conver to int
      string.remove(0, (string.indexOf(",") + 1)); // thats colour done, remove the value and the comma
    }
    if (subs.startsWith("Pal")) { // next value is if paletteMode is on or off.
      DPRINT("Pal ");
      String pal = string.substring(0, (string.indexOf(","))); // get everything until the comma
      DPRINT(pal);
      DPRINT(" - ");
      paletteMode = pal.toInt();
      DPRINTLN(paletteMode);
      string.remove(0, (string.indexOf(",") + 1)); // Remove the value
    }
    if (subs.startsWith("PNo")) { // next value is the number of the palette to use.
      DPRINT("PNo ");
      String pno = string.substring(0, (string.indexOf(","))); // get everything until the comma
      DPRINT(pno);
      DPRINT(" - ");
      paletteNumber = pno.toInt();
      DPRINTLN(pno);
      string.remove(0, (string.indexOf(",") + 1)); // Remove the value
      DPRINT("Palette set to ");
      DPRINTLN(paletteNumber);
    }
    if (subs.startsWith("Rpt")) { // next value will trigger randPattern
      DPRINT("Rpt ");
      String rpt = string.substring(0, (string.indexOf(","))); // get everything until the comma
      DPRINT(rpt);
      DPRINT(" - ");
      randTrigger = rpt.toInt();
      DPRINTLN(rpt);
      string.remove(0, (string.indexOf(",") + 1)); // Remove the value
      if (randTrigger != 0) {
        randPattern();
        randTrigger = 0;
      }
    }
    if (subs.startsWith("Rpl")) { // next value will trigger randPalette
      DPRINT("Rpl ");
      String rpl = string.substring(0, (string.indexOf(","))); // get everything until the comma
      DPRINT(rpl);
      DPRINT(" - ");
      randTrigger1 = rpl.toInt();
      DPRINTLN(rpl);
      string.remove(0, (string.indexOf(",") + 1)); // Remove the value
      if (randTrigger1 != 0) {
        randPalette();
        randTrigger1 = 0;
      }
    }
    else if (subs.startsWith("Pul")) {
      DPRINT("Pulse ");
      doPulse();
      string.remove(0, string.indexOf(",") + 1); // Remove the value
    }
    else if (subs.startsWith("Tim")) { // next value is the length of the variableDelay
      DPRINT("Tim ");
      String tim = string.substring(0, (string.indexOf(","))); // get  everything until the comma
      DPRINT(tim);
      DPRINT(" - ");
      variableDelay = tim.toInt();
      DPRINTLN(variableDelay);
      variableDelay = (constrain(variableDelay, 1, 256)); //This is constrained to prevent divide by 0 errors.
      //DPRINT("Constraned variableDelay = ");
      //DPRINTLN(variableDelay);
      string.remove(0, (string.indexOf(",") + 1)); // Remove the value
    }
    DPRINTLN(string.length()); // prints the length of the command each iteration
    DPRINT("STR = "); // prints after length < 1
    DPRINTLN(string);
  }
  string = ""; // empty it
  DPRINT("String Length == ");
  DPRINTLN(string.length());
}

void randPattern() {//choses a random pattern
  if (individualStripMode == 0) {
    patternNumber = random(2, 11);//increase by one when strobe is fixed
    DPRINT("RANDOM ");
    DPRINTLN(patternNumber);
  }
  if (individualStripMode != 0) {
    DPRINT("RANDOM ");
    int lastStripNumber = stripNumber;
    FL(1, NUM_STRIPS + 1) {
      if (i == stripNumber) {
        patternStore[i] = random(2, 11);//increase by one when strobe is fixed
        DPRINTLN(patternStore[i]);
      }
    }
    stripNumber = lastStripNumber;
  }
}
void randPalette() {//choses a random palette.
  paletteNumber = random(0, numberOfPalettes);
  DPRINT("RANDOM PALETTE ");
  DPRINTLN(paletteNumber);
}
void turnOn() {// for each LED turn it to ourCol.
  FL(LEDStart, LEDEnd) {
    leds[i] = ourCol;
  }
}
void turnOff() {//for each LED turn off.
  FL(LEDStart, LEDEnd) {
    leds[i] = CRGB(0, 0, 0);
  }
}
void doPulse() {//pulses LEDs white then turns them off.
  turnOn();
}
void theLights() { //  speckles and strobes
  int myDelay = map(variableDelay, 0, 255, (standardDelay / 8), (standardDelay * 8));
  int myFadeOut = map(variableDelay, 0, 255, (standardDelay * 8), (standardDelay / 8));
  fadeToBlackBy(&(leds[LEDStart]), NoLEDs, myFadeOut);
  int pos = random16(LEDStart, LEDEnd);
  if (paletteMode == 1) {
    colorIndex++;
    if (colorIndex > 254) {
      colorIndex = 0;
    }
    leds[pos] = ColorFromPalette( currentPalette, colorIndex, paletteBrightness, currentBlending);
  }
  else {
    leds[pos] = ourCol;
  }
  if (individualStripMode == 1) {
    delayStore[stripNumber] = myDelay;
  }
  currentDelay = myDelay;
}
void rainbow() {
  static int gHue;//rotates through the palette.
  static int pHue = 0;// as above for palette mode.
  static int hueStore;
  int myDelay = map(variableDelay, 0, 255, (standardDelay / 8), (standardDelay * 8));
  gHue++;
  if (paletteMode == 0) {
    fill_rainbow(&(leds[LEDStart]), NoLEDs, gHue, 7);
  }
  else {
    FL(LEDStart, LEDEnd) {
      pHue = i + gHue;
      if (pHue > 256) {
        hueStore = pHue - 256;
        pHue = hueStore;
      }
      leds[i] = ColorFromPalette( currentPalette, pHue, paletteBrightness, currentBlending);
    }
  }
  if (individualStripMode == 1) {
    delayStore[stripNumber] = myDelay;
  }
  currentDelay = myDelay;
}
void addGlitter( fract8 chanceOfGlitter) {
  if ( random8() < chanceOfGlitter) {
    leds[ random16(LEDStart, LEDEnd) ] += CRGB::White;
  }
}
void rainbowWithGlitter() {
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}
void confetti() {
  // random colored speckles that blink in and fade smoothly
  int myDelay = map(variableDelay, 0, 255, (standardDelay / 8), (standardDelay * 8));
  int myFadeOut = map(variableDelay, 0, 255, (standardDelay * 8), (standardDelay / 8));
  fadeToBlackBy(&(leds[LEDStart]), NoLEDs, myFadeOut);
  int pos = random16(LEDStart, LEDEnd);
  if (paletteMode == 1) {
    colorIndex++;
    if (colorIndex > 254) {
      colorIndex = 0;
    }
    leds[pos] = ColorFromPalette( currentPalette, colorIndex, paletteBrightness, currentBlending);
  }
  else {
    leds[pos] = ourCol;
    leds[pos] += CHSV( colorIndex + random8(64), 200, 255);
  }
  if (individualStripMode == 1) {
    delayStore[stripNumber] = myDelay;
  }
  currentDelay = myDelay;
}
void sinelon() {
  // a colored dot sweeping back and forth, with fading trails
  int myDelay = map(variableDelay, 0, 255, (standardDelay / 8), (standardDelay * 8));
  int myFadeOut = map(variableDelay, 0, 255, (standardDelay * 8), (standardDelay / 8));
  fadeToBlackBy(&(leds[LEDStart]), NoLEDs, myFadeOut);
  int pos = beatsin16( 13, LEDStart, LEDEnd - 1 ) ;
  if (paletteMode == 1) {
    colorIndex++;
    if (colorIndex > 254) {
      colorIndex = 0;
    }
    leds[pos] = ColorFromPalette( currentPalette, colorIndex, paletteBrightness, currentBlending);
  }
  else {
    leds[pos] = ourCol;
    leds[pos] += CHSV( colorIndex, 255, 192);
  }
  if (individualStripMode == 1) {
    delayStore[stripNumber] = myDelay;
  }
  currentDelay = myDelay;
}
void bpm() {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  int myDelay = map(variableDelay, 0, 255, (standardDelay / 6), (standardDelay * 6));
  int beatDelay = map(myDelay, (standardDelay / 6), (standardDelay * 6), (standardDelay * 6), (standardDelay / 6));
  uint8_t BeatsPerMinute = beatDelay;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  FL(LEDStart, LEDEnd) { //9948
    if (paletteMode == 1) {
      EVERY_N_MILLISECONDS(20) {
        colorIndex++;
        if (colorIndex > 254) {
          colorIndex = 0;
        }
      }
      leds[i] = ColorFromPalette(currentPalette, colorIndex + (i * 2), beat - 0 + (i * 10));
    }
    else {
      leds[i] = ourCol;
      leds[i] = ColorFromPalette(palette, colorIndex + (i * 2), beat - colorIndex + (i * 10));
    }
  }
  if (individualStripMode == 1) {
    delayStore[stripNumber] = myDelay;
  }
  currentDelay = myDelay;
}
void bouncingTrails() {
  //sets a "spawn" in the middle half of a strip then sends a trail in either direction which bounces of the ends of the strip.
  static int counter = 0; //used to set a start point at 1
  static int lastCount = 0; //i use this to be able to tell when the counter has increased. (had problems with if(counter++))
  static int paletteRef = 0; //the number of the color, in the palette, to set.
  static int posUp = 0; //holds the light that goes up the strip
  static int posDown = 0; //holds the light that goes down the strip
  static byte upReversed = 0; //if up hits the top of the strip, sets this to 1 and and runs back down the strip
  static byte downReversed = 0; //as above, in reverse
  static int pos = 0; //holds the initial position from which the two trails emerge
  int myDelay = map(variableDelay, 0, 255, (standardDelay / 2), (standardDelay * 6));
  int myFadeOut = map(variableDelay, 0, 255, (standardDelay * 6), (standardDelay / 8));
  counter++;
  if (counter == (NUM_LEDS_PER_STRIP * 4)) { //used a multiple of 2 so that a new point s formed when the two trails are close to each other (looks better in my opinon)
    counter = 0;
  }
  if (lastCount != counter) { //if the counter has progressed
    fadeToBlackBy(&(leds[LEDStart]), NoLEDs, myFadeOut);
    paletteRef = (counter * 5); //used a multiple here as we want to adress as broad a rang from 0 - 256 as possible, with more LEDs be worth lowering this value.
  }
  if ((counter == 1) && (lastCount != counter)) {
    pos = random16((NUM_LEDS_PER_STRIP / 4), ((NUM_LEDS_PER_STRIP / 4) * 3)); //if on count 1, set the spawn, and clear the reversed states.
    downReversed = 0;
    upReversed = 0;
    posUp = pos;
    posDown = pos;
    if (individualStripMode == 0) {
      FL(0, NUM_STRIPS) {
        if (paletteMode == 1) {
          leds[(pos + (i * NUM_LEDS_PER_STRIP))] = ColorFromPalette( currentPalette, paletteRef, paletteBrightness, currentBlending);
        }
        else {
          leds[(pos + (i * NUM_LEDS_PER_STRIP))] = ourCol;
        }
      }
    }
    else {//if individual strip mode is on
      if (paletteMode == 1) {
        leds[(pos + LEDStart)] = ColorFromPalette( currentPalette, paletteRef, paletteBrightness, currentBlending);
      }
      else {
        leds[(pos + LEDStart)] = ourCol;
      }
    }
  }
  if ((counter != 1) && (lastCount != counter)) { //this set of if statements increment the trailing dots, and tarcks if they are reversed
    if ((posUp != NUM_LEDS_PER_STRIP) && (upReversed == 0)) {
      posUp++;
    }
    if (posUp == NUM_LEDS_PER_STRIP - 1) {
      upReversed = 1;
    }
    if ((posUp != 0) && (upReversed == 1)) {
      posUp--;
    }
    if (posUp == 0) {
      upReversed = 0;
    }
    if ((posDown != 0) && (downReversed == 0)) {
      posDown--;
    }
    if (posDown == NUM_LEDS_PER_STRIP - 1) {
      downReversed = 0;
    }
    if (posDown == 0) {
      downReversed = 1;
      posDown++;
    }
    if ((posDown != 0) && (downReversed == 1)) {
      posDown++;
    }
    if (individualStripMode == 0) { //paint the trails
      FL(0, NUM_STRIPS) {
        if (paletteMode == 1) {
          leds[(posUp + (i * NUM_LEDS_PER_STRIP))] = ColorFromPalette( currentPalette, paletteRef,  paletteBrightness, currentBlending);
          leds[(posDown + (i * NUM_LEDS_PER_STRIP))] = ColorFromPalette( currentPalette, paletteRef, paletteBrightness, currentBlending);
        }
        else {
          leds[(posUp + (i * NUM_LEDS_PER_STRIP))] = ourCol;
          leds[(posDown + (i * NUM_LEDS_PER_STRIP))] = ourCol;
        }
      }
    }
    else {
      if (paletteMode == 1) {
        leds[(posUp + LEDStart)] = ColorFromPalette( currentPalette, paletteRef,  paletteBrightness, currentBlending);
        leds[(posDown + LEDStart)] = ColorFromPalette( currentPalette, paletteRef, paletteBrightness, currentBlending);
      }
      else {
        leds[(posUp + LEDStart)] = ourCol;
        leds[(posDown + LEDStart)] = ourCol;
      }
    }
  }
  lastCount = counter;
  if (individualStripMode == 1) {
    delayStore[stripNumber] = myDelay;
  }
  currentDelay = myDelay;
}
void juggle() {//palette not currently supported
  // eight colored dots, weaving in and out of sync with each other
  int myDelay = map(variableDelay, 0, 255, (standardDelay / 8), (standardDelay * 6));
  int myFadeOut = map(variableDelay, 0, 255, (standardDelay * 6), (standardDelay / 8));
  fadeToBlackBy(&(leds[LEDStart]), NoLEDs, myFadeOut);
  byte dothue = 0;
  if (paletteMode == 1) {
    FL(0, 8) {
      leds[beatsin16( i + 7, LEDStart, LEDEnd - 1 )] |= ColorFromPalette( currentPalette, colorIndex, paletteBrightness, currentBlending);
      leds[beatsin16( i + 7, LEDStart, LEDEnd - 1 )] |= CHSV(dothue, 200, 255);
      dothue += 32;
    }
  }
  else {
    FL(0, 8) {
      leds[beatsin16( i + 7, LEDStart, LEDEnd - 1 )] |= ourCol;
      leds[beatsin16( i + 7, LEDStart, LEDEnd - 1 )] |= CHSV(dothue, 200, 255);
      dothue += 32;
    }
  }
  if (individualStripMode == 1) {
    delayStore[stripNumber] = myDelay;
  }
  currentDelay = myDelay;
}
void simpleStrobe () {
  int myDelay = map(variableDelay, 0, 255, (standardDelay / 8), (standardDelay * 1.5));
  turnOff();
  const uint8_t kStrobeCycleLength = 6; // light every Nth frame
  static uint8_t sStrobePhase = 0;
  sStrobePhase = sStrobePhase + 1;
  colorIndex++;
  if (colorIndex > 256) {
    colorIndex = 1;
  }
  if ( sStrobePhase >= kStrobeCycleLength ) {
    sStrobePhase = 0;
  }
  if ( sStrobePhase == 0 ) {
    uint8_t dashperiod = beatsin8( 8/*cycles per minute*/, 4, 10);
    uint8_t dashwidth = (dashperiod / 4) + 1;
    uint8_t zoomBPM = STROBE_BEATS_PER_MINUTE;
    int8_t  dashmotionspeed = beatsin8( (zoomBPM / 2), 1, dashperiod);
    if (dashmotionspeed >= (dashperiod / 2)) {
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
  if (individualStripMode == 1) {
    delayStore[stripNumber] = myDelay;
  }
  currentDelay = myDelay;
}
void strobeCore(uint8_t dashperiod, uint8_t dashwidth, int8_t  dashmotionspeed, uint8_t stroberepeats, uint8_t huedelta) {
  static uint8_t sRepeatCounter = 0;
  static int8_t sStartPosition = LEDStart;
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
    }
    else if ( sStartPosition < 0) {
      while ( sStartPosition < 0) {
        sStartPosition += dashperiod;
      }
      sStartHue  += huedelta;
    }
  }
  const uint8_t kSaturation = 208; // WHITE >> CURRENT COLOUR control (def 208)
  const uint8_t kValue = 200; // Brightness??
  strobeDraw( sStartPosition, LEDEnd - 1, dashperiod, dashwidth, sStartHue, huedelta, kSaturation, kValue);
}
static void strobeDraw(uint8_t startpos, uint16_t lastpos, uint8_t period, uint8_t width, uint8_t huestart, uint8_t huedelta, uint8_t saturation, uint8_t value) {
  uint8_t hue = huestart;
  for ( uint16_t i = startpos; i <= lastpos; i += period) {
    CRGB color = CHSV( hue, saturation, value);
    //CRGB color = CRGB::Blue; // USE TO COMPLETELY BYPASS HSV Change Scheme
    uint16_t pos = i;
    for ( uint8_t w = 0; w < width; w++) {
      leds[pos] = ColorFromPalette(currentPalette, colorIndex);
      pos++;
      if (pos >= LEDEnd) {
        break;
      }
    }
    hue += huedelta;
  }
}
void paletteSelect() {//selects palette
  switch (paletteNumber) {
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
}

void patternSelect() {//selects pattern
  int lastStripNumber = stripNumber;
  if (individualStripMode == 0) {
    setLEDs();
    switch (patternNumber) {
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
        bouncingTrails();
        break;
      case 9:
        juggle();
        break;
      case 10:
        simpleStrobe();
        break;
    }
  }
  if (individualStripMode == 1) { //second switch case used to set patternSelect array.
    FL(1, NUM_STRIPS + 1) {
      stripNumber = i;
      setLEDs();
      y = patternStore[i];
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
          bouncingTrails();
          break;
        case 9:
          juggle();
          break;
        case 10:
          simpleStrobe();
          break;
      }
    }
    stripNumber = lastStripNumber;
  }
}
void setup() {
  Wire.begin(I2C_ADDR);
  Wire.onReceive(receiveEvent);
  pinMode(DEBUG_LED, OUTPUT);
  DBEGIN(9600);
  DPRINTLN("Ready for i2c");
  //sets one long array containing multiple data pins in the following format.
  //type of led/ data pin/ color order(if not RGB)/ name/ point in array to start adding from/ number of LED's to add.
  FastLED.addLeds<WS2812B, 2, GRB>(leds, 0, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, 3, GRB>(leds, NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, 4, GRB>(leds, 2 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, 5, GRB>(leds, 3 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.setBrightness(BRIGHTNESS);
  currentBlending = LINEARBLEND;
  int i = 0;
  while (i < NUM_STRIPS + 1) {
    ourCol = startup[i];
    setLEDs();
    turnOn();
    FastLED.show();
    FastLED.delay(300);
    i++;
  }
  FL(1, NUM_STRIPS + 1) {
    patternStore[i] = (2 + i); //sets an initial value for pattern to use in indivdual strip mode.
  }
  turnOff();
  FastLED.show();
}

void loop() {
#ifdef DEBUG//serial input for debug purposes.
  if (Serial.available()) {
    string = "";
    while (Serial.available()) {
      char c = Serial.read();
      string.concat(c);
      delay(50);
    }
    if (string != '\0') {
      parser();
      string = "";
    }
  }
#endif
  EVERY_N_SECONDS(1) {
    timeSinceBt++;
  }
  if (timeSinceBt >= autoSecs) {
    switch (autoMode) {
      case 0:
        break;
      case 1:
        randPalette();
        randPattern();
        timeSinceBt = 0;
        break;
      case 2:
        randPattern();
        timeSinceBt = 0;
        break;
      case 3:
        randPalette();
        timeSinceBt = 0;
        break;
    }
  }
  setLEDs();
  paletteSelect();
  patternSelect();
  //FastLED.show();
  if (individualStripMode == 1) {
    int sum = delayStore[1] + delayStore[2] + delayStore[3] + delayStore[4];
    float avg = sum * 0.25;
    FastLED.delay((int)avg);
  }
  else FastLED.delay(currentDelay);
}
