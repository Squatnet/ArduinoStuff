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
#define I2C_ADDR 3

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
int autoSecs = 5;//sets the upper bound for timeSinceBt function.
int stripNumber = 1;//stores the strip that we wish to set the pattern on.
int individualStripMode = 0;//holds wether we are addressing   all the stips(0)or individual strips (1)
int paletteMode = 1;//holds if we sending indivdual colors to the patterns or a palette array.
int paletteNumber = 0;//holds the number for which palette is in use when paletteMode is on.
int numberOfPalettes = 1; //total number of palettes available -1.
int colorIndex = 0;//holds the position in the palette array for the color to show.
int LEDStart = 0;//this holds the number of the first LED in the arry to start printing a pattern to.
int LEDEnd = 0;//this holds the number of the last LED in the arry to start printing a pattern to.
int NoLEDs = 0;//this holds how many LED's we need to address. (may be a better way of doin this ToDo)
int patternStore[NUM_STRIPS + 1]; //this array holds the pattern number for each strip.
int standardDelay = 20;//initial delay that the maths of the variable below relates to.
long variableDelay = 128; //0..256 visible to parser and used to set FastLED.delay values.
uint8_t brightness = 255;
bool debugLED = false;//when true will flash the onboard LED each time the loop compleates.

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
//DPRINTLN("setLEDs Called");
  if (individualStripMode == 1) {
    NoLEDs = NUM_LEDS_PER_STRIP;//if we are adressing individual strips  the number of LEDs to change will be equal to the number of LEDs in one strip.
    FL(1, NUM_STRIPS + 1) { //loops through each strip
      if ((individualStripMode == 1) && (stripNumber == i)) {//if it is equal to the strip we wish to address, set the start and end number of the strip.
        LEDEnd = (NUM_LEDS_PER_STRIP * i);
	//	DPRINT("LEDEnd = ");
	//	DPRINTLN(LEDEnd);
        LEDStart = ((NUM_LEDS_PER_STRIP * i) - (NUM_LEDS_PER_STRIP));
	//	DPRINT("LEDStart = ");
	//	DPRINTLN(LEDStart);
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
    if (subs.startsWith("Ats")) { // next value is autoSecs
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
      int tester;
      bool trigger = 0;
      tester = sno.toInt();
      if (tester > NUM_STRIPS or tester <= 0) { //catches for unexpected numbers of strips.
        trigger = 1;
        tester = 1;
      }
        stripNumber = tester;
        DPRINTLN(stripNumber);
        string.remove(0, (string.indexOf(",") + 1)); // Remove the value
        setLEDs();
    }
    if (subs.startsWith("Ptn")) { // next value is pattern.
    DPRINT("PTN ");
    String ptn = string.substring(0, (string.indexOf(","))); // get everything until the comma
    DPRINT(ptn);
    DPRINT(" - ");
    patternNumber = ptn.toInt(); // Its going to be an integer. its the pattern number,
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
      paletteMode = pal.toInt(); // Its going to be an integer. its paletteMode
      DPRINTLN(paletteMode);
      string.remove(0, (string.indexOf(",") + 1)); // Remove the value
    }
    if (subs.startsWith("PNo")) { // next value is the number of the palette to use.
      DPRINT("PNo ");
      String pno = string.substring(0, (string.indexOf(","))); // get everything until the comma
      DPRINT(pno);
      DPRINT(" - ");
      paletteNumber = pno.toInt(); // Its going to be an integer. its the palette number.
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
      randTrigger = rpt.toInt(); // Its going to be an integer. its the palette number.
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
      randTrigger1 = rpl.toInt(); // Its going to be an integer. its the palette number.
      DPRINTLN(rpl);
      string.remove(0, (string.indexOf(",") + 1)); // Remove the value
      if (randTrigger1 != 0) {
        randPalette();
        randTrigger1 = 0;
      }
    }
    /*else if (subs.startsWith("Pul")) {
      DPRINT("Pulse ");
      doPulse();
      string.remove(0, string.indexOf(",") + 1); // Remove the value
    }*/
    else if (subs.startsWith("Tim")) { // next value is the length of the variableDelay
      DPRINT("Tim ");
      String tim = string.substring(0, (string.indexOf(","))); // get  everything until the comma
      DPRINT(tim);
      DPRINT(" - ");
      variableDelay = tim.toInt(); // Its going to be an integer. its the mirror number.
      DPRINTLN(variableDelay);
      //variableDelay=(constrain(variableDelay,0,255));
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
    patternNumber = random(2, 4);
    DPRINT("RANDOM ");
    DPRINTLN(patternNumber);
  }
  if (individualStripMode != 0) {
   DPRINT("RANDOM ");
    int lastStripNumber = stripNumber;
    FL(1, NUM_STRIPS + 1) {
    if (i == stripNumber) {
      patternStore[i] = random(2, 4);
      DPRINTLN(patternStore[i]);
    }
    }
    stripNumber = lastStripNumber;
  }
}
void randPalette() {
  paletteNumber = random(0, 2);
  DPRINT("RANDOM PALETTE ");
  DPRINTLN(paletteNumber);
}
void turnOn() {// for each LED turn it to ourCol.
 // DPRINTLN("TurnOn Start");
  FL(LEDStart, LEDEnd) {
    leds[i] = ourCol;
  }
  //FastLED.show();
    FastLED.delay(standardDelay);
}
void turnOff() {//for each LED turn off.
  //DPRINTLN("TurnOff Start");
  FL(LEDStart, LEDEnd) {
    leds[i] = CRGB(0, 0, 0);
  }
    FastLED.delay(standardDelay);
}
void theLights() { //  speckles and strobes
  //DPRINTLN("The Lights Start");
  int myDelay = map(variableDelay, 0, 255, (standardDelay / 8), (standardDelay * 8));
  if (individualStripMode==1){
	  myDelay=(myDelay/4);
  }
  int myFadeOut = map(variableDelay, 0, 255, (standardDelay * 8), (standardDelay / 8));
  fadeToBlackBy(&(leds[LEDStart]), NoLEDs, myFadeOut);
  int pos = random16(LEDStart, LEDEnd);
  if (paletteMode == 1) {
    //DPRINTLN("The Lights palette On");
    colorIndex++;
    if (colorIndex > 254) {
      colorIndex = 0;
    }
    leds[pos] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
  }
  else {
    //DPRINTLN("The Lights palette Off");
    leds[pos] = ourCol;
  }
    FastLED.delay(myDelay);
}
void rainbow() {
  //DPRINTLN("Rainbow Start");
  // FastLED's built-in rainbow generator
  static int gHue;//rotates through the palette.
  static int pHue = 0;
  static int hueStore;
  int myDelay = map(variableDelay, 0, 255, (standardDelay / 8), (standardDelay * 8));
  if (individualStripMode==1){
	  myDelay=(myDelay/4);
  }
  gHue++;
  if (paletteMode == 0) {
    //DPRINTLN("Rainbow palette Off");
    fill_rainbow(&(leds[LEDStart]), NoLEDs, gHue, 7);
  }
  else {
    //DPRINTLN("Rainbow palette On");
    FL(LEDStart, LEDEnd) {
      pHue = i + gHue;
      if (pHue > 256) {
        hueStore = pHue - 256;
        pHue = hueStore;
      }
      leds[i] = ColorFromPalette( currentPalette, pHue, brightness, currentBlending);
    }
  }
    FastLED.delay(myDelay);
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

void patternSelect() {
	//DPRINT("individualStripMode = ");
	//DPRINTLN(individualStripMode);
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
    }
  }
	if (individualStripMode == 1) { //second switch case used to set patternSelect array.
    FL(1, NUM_STRIPS + 1) {
      stripNumber = i;
      setLEDs();
      y = patternStore[i];
     /* DPRINT("Pattern Array ");
      DPRINT(i);
      DPRINT(" = ");
      DPRINTLN(y);*/
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
      }
    }
	stripNumber=lastStripNumber;
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
  FastLED.setBrightness(128);
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
	/*DPRINT("patternStore = ");
	FL(1, NUM_STRIPS + 1) {
    patternStore[i] = 2;
	DPRINT(patternStore[i]);
  }*/
  
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
      delay(5);
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
   /* if (autoMode != 0){
      setLEDs();
      paletteSelect();
      patternSelect();
      timeSinceBt = 0;
    }*/
    
 
      setLEDs();
      paletteSelect();
      patternSelect();
      
  /*if (individualStripMode != 0) {
    FastLED.delay(standardDelay);
  }*/
 
}
