#include <FastLED.h>
#include <Wire.h>
#include <LEDMatrix.h>
#include <LEDText.h>
//#include <FontMatrise.h> // uncomment for text
// CHANGE THIS FOR ALTERNATE MATRIX
#define I2C_ADDR 2

#define FL(aa,bb) for (int i = aa; i < bb; i++)
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

#define LED_PIN        6
#define COLOR_ORDER    GRB
#define CHIPSET        WS2812B
#define MATRIX_WIDTH   8
#define MATRIX_HEIGHT  8
#define NUM_LEDS       64
#define MATRIX_TYPE    HORIZONTAL_ZIGZAG_MATRIX
#define FRAMES_PER_SECOND  120
#define STROBE_BEATS_PER_MINUTE 97.5
#define CONNECTED_STRIPS 1

cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> matrix;
//cLEDText ScrollingMsg;
CRGB ourCol = CRGB(255, 255, 255);
CRGBPalette16 currentPalette;//holds the palette
TBlendType currentBlending;//blending type
char TxtAncs[] = {" ANCS - CUSTOM BUILT AUDIO / VISUAL - WWW.AUTOAV.GQ "};
String string = "";
int iic = 0; // holder for i2c message
int timeSinceBt = 0;
int autoMode = 0;
int autoSecs = 5;//time between autoMode changes.
int colorIndex = 0;//holds the position in the palette array for the color to show.
int mirrorNumber = 0;//mirror mode number.
int standardDelay = 20;//initial delay that the maths of the variable below relates to.
int variableDelay = 128; //0..256 visible to parser and used to set FastLED.delay values.
uint8_t brightness = 255;
byte patternNumber = 0;
byte paletteMode = 1;//holds if we sending indivdual colors to the patterns or a palette array.
byte paletteNumber = 1;//holds the number for which palette is in use when paletteMode is on.
byte numberOfPalettes = 1; //total number of palettes available -1.

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
void setup() {
  Wire.begin(I2C_ADDR); //2,3,4 for A,B+C aNCS_2560 boards.
  Wire.onReceive(receiveEvent);
  DBEGIN(9600);
  randomSeed(analogRead(0));
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(matrix[0], matrix.Size());
  FastLED.setBrightness(64); // Limit Power Consumption
  currentBlending = LINEARBLEND;
  FastLED.clear(true);
  /*ScrollingMsg.SetFont(MatriseFontData); // or Robotron.  uncommeent block to initialize text.
    ScrollingMsg.Init(&matrix, matrix.Width(), ScrollingMsg.FontHeight() + 1, 0, 0);
    ScrollingMsg.SetText((unsigned char *)TxtAncs, sizeof(TxtAncs) - 1);
    ScrollingMsg.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 0xff, 0xff, 0xff);
    ScrollingMsg.SetScrollDirection(SCROLL_LEFT);
    ScrollingMsg.SetFrameRate(2);*/
  delay(500);
  DPRINTLN("SETUP");
}
void(* resetFunc) (void) = 0; // Software reset hack

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

void parser() {
  DPRINTLN("PARSER");
  byte randTrigger = 0; //initializes a randPattern trigger (didn't need to be global);
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
    if (subs.startsWith("Ptn")) { // next value is pattern.
      DPRINT("PTN ");
      String ptn = string.substring(0, string.indexOf(",")); // get everything until the comma
      DPRINT(ptn);
      DPRINT(" - ");
      patternNumber = ptn.toInt(); // Its going to be an integer. its the pattern number,
      DPRINTLN(patternNumber);
      string.remove(0, string.indexOf(",") + 1); // Remove the value
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
    if (subs.startsWith("Pal")) { // next value is if paletteMode is on or off.
      DPRINT("Pal ");
      String pal = string.substring(0, string.indexOf(",")); // get everything until the comma
      DPRINT(pal);
      DPRINT(" - ");
      paletteMode = pal.toInt(); // Its going to be an integer. its paletteMode
      DPRINTLN(paletteMode);
      string.remove(0, string.indexOf(",") + 1); // Remove the value
    }
    if (subs.startsWith("PNo")) { // next value is the number of the palette to use.
      DPRINT("PNo ");
      String pno = string.substring(0, string.indexOf(",")); // get  everything until the comma
      DPRINT(pno);
      DPRINT(" - ");
      paletteNumber = pno.toInt(); // Its going to be an integer. its the palette number.
      DPRINTLN(paletteNumber);
      string.remove(0, string.indexOf(",") + 1); // Remove the value
    }
    if (subs.startsWith("Ran")) { // next value will trigger randPattern
      DPRINT("Ran ");
      String ran = string.substring(0, string.indexOf(",")); // get  everything until the comma
      DPRINT(ran);
      DPRINT(" - ");
      randTrigger = ran.toInt(); // Its going to be an integer. 1 triggers randPattern
      DPRINTLN(randTrigger);
      string.remove(0, string.indexOf(",") + 1); // Remove the value
      if (randTrigger != 0) {
        randPattern();
        randTrigger = 0;
      }
    }
    if (subs.startsWith("Msg")) { // next value will be the message to display
      DPRINT("Msg ");
      String Msg = string.substring(0, string.indexOf(",")); // get everything until the comma
      DPRINT(Msg);
      DPRINT(" - ");
      /*FL(0,(sizeof(TxtAncs))){
      	TxtAncs[i]='\0';
        }*/
      memset(TxtAncs, 0, sizeof TxtAncs);
      string.toCharArray(TxtAncs, string.length());
      DPRINTLN(TxtAncs);
      string.remove(0, string.indexOf(",") + 1); // Remove the value
    }
    if (subs.startsWith("Mir")) { // next value is the number of the mirror mode to use. 0==off.
      DPRINT("Mir ");
      String mir = string.substring(0, string.indexOf(",")); // get  everything until the comma
      DPRINT(mir);
      DPRINT(" - ");
      mirrorNumber = mir.toInt(); // Its going to be an integer. its the mirror number.
      DPRINTLN(mirrorNumber);
      string.remove(0, string.indexOf(",") + 1); // Remove the value
    }
    if (subs.startsWith("Del")) { // next value is the length of the variableDelay
      DPRINT("Del ");
      String del = string.substring(0, string.indexOf(",")); // get  everything until the comma
      DPRINT(del);
      DPRINT(" - ");
      variableDelay = constrain(del.toInt(), 0, 256); // Its going to be an integer. its the mirror number.
      DPRINTLN(variableDelay);
      string.remove(0, string.indexOf(",") + 1); // Remove the value
    }
    DPRINTLN(string.length()); // prints the length of the command each iteration
    DPRINT("STR = "); // prints after length < 1
    DPRINTLN(string);
    string = ""; // empty it
  }
}
////////////////////////start of patterns///////////////////////
void turnOff() {//for each LED turn off.
  FL(0, NUM_LEDS) {
    matrix(i) = CRGB( 0, 0, 0);
    //FastLED.delay(standardDelay);
  }
}
void turnOn() {// for each LED turn it to ourCol.
  FL(0, NUM_LEDS) {
    matrix(i) = ourCol;
  }
  // FastLED.delay(standardDelay);
}
void newPattern() {
  int myDelay = map(variableDelay, 0, 256, (standardDelay / 4), (standardDelay * 4));
  //int myFadeOut = map(variableDelay, 0, 256, (standardDelay * 10), (standardDelay * 2));
    fadeToBlackBy(&(matrix(0)), NUM_LEDS, 12);
  int pos = 0;
  static CRGB patternArray[NUM_LEDS];
  static CRGB patternStore[NUM_LEDS];
  static int lineCounter = 0;
  static int moduloCounter = 2;
  FL(pos,NUM_LEDS){
	  if (pos%moduloCounter==0&&lineCounter==0){
		  patternArray[pos]=ColorFromPalette(currentPalette,colorIndex,brightness,currentBlending);
	  }
  }
  FL(0,NUM_LEDS){
	  patternStore[i]=(patternArray[i+1]);
	  if (i==NUM_LEDS){
		  patternStore[0]=(patternArray[NUM_LEDS]);
	  }
  }
/*  EVERY_N_SECONDS(autoSecs) {
    moduloCounter = random16(2, 6);
  }
  EVERY_N_SECONDS(autoSecs / 4) {
    lineCounter++;
    if (lineCounter == 2) {
      lineCounter = 0;
    }
    colorIndex = (colorIndex + 10);
    if (colorIndex > 255) {
      colorIndex = 0;
    }
    for (pos = 0; pos < NUM_LEDS; pos++) {
      if (pos % moduloCounter == 0 && lineCounter == 0) {
        matrix(pos) = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
      }
      if (pos % moduloCounter == 1 && lineCounter == 1) {
        matrix(pos) = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
      }
    }
  }*/
  EVERY_N_SECONDS(autoSecs / 5){
    matrix.ShiftLeft();
  }
  FastLED.delay(myDelay);
  //turnOff();
}
void randPattern() { //choses a random pattern
  patternNumber = random(2, 13);
  DPRINT("RANDOM PATTERN ");
  DPRINTLN(patternNumber);
}
void randPalette() {
  paletteNumber = random(2, numberOfPalettes);
  DPRINT("RANDOM PALETTE ");
  DPRINTLN(paletteNumber);
}
void randMirror() {
  mirrorNumber = random(0, 3);
  DPRINT("RANDOM MIRROR ");
  DPRINTLN(mirrorNumber);
}
void paletteSelect() {
  switch (paletteNumber) {
    case 0:
      currentPalette = Pastel1_08_gp;
      break;
    case 1:
      currentPalette = Paired_08_gp;
      break;
  }
}
void mirrorSelect() {
  switch (mirrorNumber) {
    case 0:
      break;
    case 1:
      matrix.QuadrantMirror();
      break;
    case 2:
      matrix.VerticalMirror();
      break;
    case 3:
      matrix.HorizontalMirror();
      break;
  }
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
  /*EVERY_N_SECONDS(1) {
  		timeSinceBt++;//count the time since beat
    }
    if (autoMode == 1) { //if auto mode on cycles palettes, patterns and mirror mode.
  		if (timeSinceBt >= autoSecs) {
  			timeSinceBt = 0;
  			randPalette();
  			randPattern();
  			randMirror();
  		}
  	}
    if (autoMode==2){ //if automode 2 cycles a random pattern.
  	if (timeSinceBt >= autoSecs) {
  		timeSinceBt=0;
  		randPattern();
  		randPalette();
  	}
    }
    if (autoMode==3){//if automode 3, cycles a random palette.
  	if (timeSinceBt >= autoSecs) {
  		timeSinceBt=0;
  		randPattern();
  	}
    }
    if (autoMode==4){//if automode 3, cycles a random palette.
  	if (timeSinceBt >= autoSecs) {
  		timeSinceBt=0;
  		randPalette();
  	}
    }*/
  paletteSelect();
  newPattern();
  //patternSelect();
}
