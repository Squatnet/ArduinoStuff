#include <FastLED.h>
#include <Wire.h>
#include <LEDMatrix.h>
#include <LEDText.h>
#include <FontMatrise.h>
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

#define LED_PIN        3
#define COLOR_ORDER    GRB
#define CHIPSET        WS2812B
#define MATRIX_WIDTH   32
#define MATRIX_HEIGHT  8
#define NUM_LEDS       256
#define MATRIX_TYPE    HORIZONTAL_ZIGZAG_MATRIX
#define FRAMES_PER_SECOND  120
#define STROBE_BEATS_PER_MINUTE 97.5
#define CONNECTED_STRIPS 1

cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> matrix;
cLEDText ScrollingMsg;
CRGB ourCol = CRGB(255, 255, 255);
CRGBPalette16 currentPalette;//holds the palette
TBlendType currentBlending;//blending type 
char TxtAncs[160] = {" ANCS - CUSTOM BUILT AUDIO / VISUAL - WWW.ANCS.GQ    "};
int iic = 0; // holder for i2c message
String string = "";
int timeSinceBt = 0;
int autoMode = 0;
int autoSecs = 10;
byte patternNumber = 0;
byte paletteMode = 1;//holds if we sending indivdual colors to the patterns or a palette array.
byte paletteNumber = 0;//holds the number for which palette is in use when paletteMode is on.
byte numberOfPalettes =18;//total number of palettes available -1.
int colorIndex = 0;//holds the position in the palette array for the color to show.
uint8_t brightness = 255;
int mirrorNumber = 0;
int standardDelay = 20;
int variableDelay = 128;

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
         
void setup() {
	Wire.begin(I2C_ADDR); //2,3,4 for A,B+C aNCS_2560 boards.
	Wire.onReceive(receiveEvent);
	DBEGIN(115200);
	randomSeed(analogRead(0));
	FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(matrix[0], matrix.Size());
	FastLED.setBrightness(64); // Limit Power Consumption 
	currentBlending = LINEARBLEND;
	FastLED.clear(true);
	ScrollingMsg.SetFont(MatriseFontData); // or Robotron.
	ScrollingMsg.Init(&matrix, matrix.Width(), ScrollingMsg.FontHeight() + 1, 0, 0);
	ScrollingMsg.SetText((unsigned char *)TxtAncs, sizeof(TxtAncs) - 1);
	ScrollingMsg.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 0xff, 0xff, 0xff);
	ScrollingMsg.SetScrollDirection(SCROLL_LEFT);
	ScrollingMsg.SetFrameRate(2);
	FastLED.showColor(CRGB::Red);
	delay(500);
	FastLED.showColor(CRGB::Green);
	delay(500);
	FastLED.showColor(CRGB::Blue);
	delay(500);
	FastLED.show();
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
	byte randTrigger=0;//initializes a randPattern trigger (didn't need to be global);
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
			String pno= string.substring(0, string.indexOf(",")); // get  everything until the comma
			DPRINT(pno);
			DPRINT(" - ");
			paletteNumber = pno.toInt(); // Its going to be an integer. its the palette number.
			DPRINTLN(paletteNumber);
			string.remove(0, string.indexOf(",") + 1); // Remove the value
		}
		if (subs.startsWith("Ran")) { // next value will trigger randPattern
			DPRINT("Ran ");
			String ran= string.substring(0, string.indexOf(",")); // get  everything until the comma
			DPRINT(ran);
			DPRINT(" - ");
			randTrigger = ran.toInt(); // Its going to be an integer. 1 triggers randPattern
			DPRINTLN(randTrigger);
			string.remove(0, string.indexOf(",") + 1); // Remove the value
			if (randTrigger!=0){
				randPattern();
				randTrigger=0;
			}
		}
		if (subs.startsWith("Msg")) { // next value will be the message to display
			DPRINT("Msg ");
			String Msg= string.substring(0, string.indexOf(",")); // get everything until the comma
			DPRINT(Msg);
			DPRINT(" - ");
			string.toCharArray(TxtAncs, string.length());
			DPRINTLN(TxtAncs);
			string.remove(0, string.indexOf(",") + 1); // Remove the value
		}
		if (subs.startsWith("Mir")) { // next value is the number of the mirror mode to use. 0==off.
			DPRINT("Mir ");
			String mir= string.substring(0, string.indexOf(",")); // get  everything until the comma
			DPRINT(mir);
			DPRINT(" - ");
			mirrorNumber = mir.toInt(); // Its going to be an integer. its the mirror number.
			DPRINTLN(mirrorNumber);
			string.remove(0, string.indexOf(",") + 1); // Remove the value
		}
		if (subs.startsWith("Del")) { // next value is the length of the variableDelay
			DPRINT("Del ");
			String del= string.substring(0, string.indexOf(",")); // get  everything until the comma
			DPRINT(del);
			DPRINT(" - ");
			variableDelay = constrain(del.toInt(),0,256); // Its going to be an integer. its the mirror number.
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
void turnOn() {// for each LED turn it to ourCol.
	FL(0, NUM_LEDS) {
		matrix(i) = ourCol;
	}
	FastLED.delay(standardDelay);
}
void turnOff() {//for each LED turn off.
	FL(0, NUM_LEDS) {
		matrix(i) = CRGB( 0, 0, 0);
		FastLED.delay(standardDelay);
	}
}
void randPattern(){//choses a random pattern
	patternNumber = random(2, 13);
	DPRINT("RANDOM PATTERN ");
	DPRINTLN(patternNumber);
}
void randPalette(){
	paletteNumber=random(2,numberOfPalettes);
	DPRINT("RANDOM PALETTE ");
	DPRINTLN(paletteNumber);
}
void randMirror(){
	mirrorNumber=random(0,4);
	DPRINT("RANDOM MIRROR ");
	DPRINTLN(mirrorNumber);
}

void solidWhite(){
	fill_solid(&(matrix(0)),NUM_LEDS,CRGB::White);//possible error
}
void theLights() { //  speckles and strobes
	int myDelay = map(variableDelay,0,256,(standardDelay/8),(standardDelay*8));
	fadeToBlackBy(&(matrix(0)), NUM_LEDS, 10);
	int pos = random16(0, NUM_LEDS);
	if (paletteMode==1){
		colorIndex++;
		if(colorIndex>254){
			colorIndex=0;
		}
		matrix(pos) = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
	}
	else{
		matrix(pos) = ourCol;
	}
	FastLED.delay(myDelay);
}
void rainbow(){
	// FastLED's built-in rainbow generator
	static int gHue;//rotates through the palette.
	static int pHue=0;
	static int hueStore;
	int myDelay = map(variableDelay,0,256,(standardDelay/8),(standardDelay*8));
	gHue++;
	if (gHue>256){
		gHue=0;
	}
	if (paletteMode==0){
		fill_rainbow(&(matrix(0)),NUM_LEDS,gHue,7);
	}
	else{
		FL(0,NUM_LEDS){
			pHue=i+gHue;
			if (pHue>256){
				hueStore=pHue-256;
				pHue=hueStore;
			}
			matrix(i)=ColorFromPalette(currentPalette,pHue,brightness,currentBlending);
		}
	}
	FastLED.delay(myDelay);
}
void addGlitter( fract8 chanceOfGlitter){
	int myDelay = map(variableDelay,0,256,(standardDelay/8),(standardDelay*8));
	if (random8()<chanceOfGlitter) {
		matrix(random16(0,NUM_LEDS))+=CRGB::White;//possible error
	}
	FastLED.delay(myDelay);
}
void rainbowWithGlitter(){
	int myDelay = map(variableDelay,0,256,(standardDelay/8),(standardDelay*8));
	// built-in FastLED rainbow, plus some random sparkly glitter
	rainbow();
	addGlitter(80);
	FastLED.delay(myDelay);
}
void confetti(){
	// random colored speckles that blink in and fade smoothly
	int myDelay = map(variableDelay,0,256,(standardDelay/8),(standardDelay*8));
	fadeToBlackBy(&(matrix(0)), NUM_LEDS, 10);
	int pos = random16(0, NUM_LEDS);
	if (paletteMode==1){
		colorIndex++;
		if(colorIndex>254){
			colorIndex=0;
		}
		matrix(pos) = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
	}
	else {
		matrix(pos) = ourCol;
		matrix(pos) += CHSV( colorIndex + random8(64), 200, 255);
	}
	FastLED.delay(myDelay);
}
void sinelon(){
	// a colored dot sweeping back and forth, with fading trails
	int myDelay = map(variableDelay,0,256,(standardDelay/8),(standardDelay*8));
	fadeToBlackBy(&(matrix(0)), NUM_LEDS, 20);
	int pos = beatsin16( 13, 0, NUM_LEDS - 1 ) ;
	if (paletteMode==1){
		colorIndex++;
		if(colorIndex>254){
			colorIndex=0;
		}
		matrix(pos) = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
	}
	else{
		matrix(pos) = ourCol;
		matrix(pos) += CHSV( colorIndex, 255, 192);
	}
	FastLED.delay(myDelay);
}
void bpm(){
	// colored stripes pulsing at a defined Beats-Per-Minute (BPM)
	int myDelayLow = standardDelay/6;
	int myDelayHigh = standardDelay*6;
	int myDelay = map(variableDelay,0,256,myDelayLow,myDelayHigh);
	int beatDelay = map(myDelay,myDelayLow,myDelayHigh,myDelayHigh,myDelayLow);
	uint8_t BeatsPerMinute = beatDelay;
	CRGBPalette16 palette = PartyColors_p;
	uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
	FL(0, NUM_LEDS) { //9948 
		if (paletteMode==1){
			EVERY_N_MILLISECONDS(20){
				colorIndex++;
				if(colorIndex>254){
					colorIndex=0;
				}
			}
			matrix(i) = ColorFromPalette(currentPalette, colorIndex + (i * 2), beat - 0 + (i * 10));
		}
		else{
			matrix(i) = ourCol;
			matrix(i) = ColorFromPalette(palette, colorIndex + (i * 2), beat - colorIndex + (i * 10));
		}
	}
	FastLED.delay(myDelay);
}
void simpleStrobe () {
	int myDelay = map(variableDelay,0,256,(standardDelay/8),(standardDelay*1.5));
	fill_solid(&(matrix(0)), NUM_LEDS, CRGB::Black);
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
	FastLED.delay(myDelay);
}
void strobeCore(uint8_t dashperiod, uint8_t dashwidth, int8_t  dashmotionspeed, uint8_t stroberepeats, uint8_t huedelta) {
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
		}
		else if( sStartPosition < 0) {
			while( sStartPosition < 0) { sStartPosition += dashperiod; }
				sStartHue  += huedelta;
		}
	}
	const uint8_t kSaturation = 208; // WHITE >> CURRENT COLOUR control (def 208)
	const uint8_t kValue = 200; // Brightness??
	strobeDraw( sStartPosition, NUM_LEDS-1, dashperiod, dashwidth, sStartHue, huedelta, kSaturation, kValue);
}
static void strobeDraw( uint8_t startpos, uint16_t lastpos, uint8_t period, uint8_t width, uint8_t huestart, uint8_t huedelta, uint8_t saturation, uint8_t value){
	uint8_t hue = huestart;
	for( uint16_t i = startpos; i <= lastpos; i += period) {
		CRGB color = CHSV( hue, saturation, value);  
		//CRGB color = CRGB::Blue; // USE TO COMPLETELY BYPASS HSV Change Scheme
		uint16_t pos = i;
		for( uint8_t w = 0; w < width; w++) {
			matrix(pos) = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
			pos++;
			if( pos >= NUM_LEDS) {
				break;
			}
		}
		hue += huedelta;
	}
}
void bouncingTrails(){
	//sets a "spawn" in the middle half of a strip then sends a trail in either direction which bounces of the ends of the strip.
	static int counter=0;//used to set a start point at 1
	static int lastCount=0;//i use this to be able to tell when the counter has increased. (had problems with if(counter++))
	static int paletteRef=0;//the number of the color, in the palette, to set.
	static int posUp=0;//holds the light that goes up the strip
	static int posDown=0;//holds the light that goes down the strip
	static byte upReversed=0;//if up hits the top of the strip, sets this to 1 and and runs back down the strip
	static byte downReversed=0;//as above, in reverse
	static int pos=0;//holds the initial position from which the two trails emerge
	int myDelay = map(variableDelay,0,256,(standardDelay/8),(standardDelay*6));
	counter++;
	if (counter==(MATRIX_WIDTH*4)){//used a multiple of 2 so that a new point s formed when the two trails are close to each other (looks better in my opinon)
		counter=0;
	}
	if (lastCount!=counter){//if the counter has progressed
		fadeToBlackBy(&(matrix(0)), NUM_LEDS, 60);
		paletteRef=(counter*5);//used a multiple here as we want to adress as broad a rang from 0 - 256 as possible, with more LEDs be worth lowering this value.
	}
	if ((counter==1)&&(lastCount!=counter)){   
		pos = random16((MATRIX_WIDTH/4),((MATRIX_WIDTH/4)*3));//if on count 1, set the spawn, and clear the reversed states.
		downReversed=0;
		upReversed=0;
		posUp=pos;
		posDown=pos;
		FL(0,MATRIX_HEIGHT){
			if (paletteMode==1){
				matrix((pos+(i*MATRIX_WIDTH)))=ColorFromPalette( currentPalette, paletteRef, brightness, currentBlending);
			}
			else{
				matrix((pos+(i*MATRIX_WIDTH)))=ourCol;
			}	 
		}
	}  
    if ((counter!=1)&&(lastCount!=counter)){//this set of if statements increment the trailing dots, and tarcks if they are reversed
		if ((posUp!=MATRIX_WIDTH)&&(upReversed==0)){
			posUp++;
		}
		if (posUp==MATRIX_WIDTH){
			upReversed=1;
		}
		if ((posUp!=0)&&(upReversed==1)){
			posUp--;
		}
		if (posUp==0){
			upReversed=0;
		}
		if ((posDown!=0)&&(downReversed==0)){
			posDown--;
		}
		if (posDown==MATRIX_WIDTH){
			downReversed=0;
		}
		if (posDown==0){
			downReversed=1;
			posDown++;
		}
		if ((posDown!=0)&&(downReversed==1)){
			posDown++;
		}
		FL(0,MATRIX_HEIGHT){
			if (paletteMode==1){
				matrix((posUp+(i*MATRIX_WIDTH)))=ColorFromPalette( currentPalette, paletteRef,  brightness, currentBlending);
				matrix((posDown+(i*MATRIX_WIDTH)))=ColorFromPalette( currentPalette, paletteRef, brightness, currentBlending);
			}
			else{
				matrix((posUp+(i*MATRIX_WIDTH)))=ourCol;
				matrix((posDown+(i*MATRIX_WIDTH)))=ourCol;
			}
		}
	}
	lastCount=counter;
	FastLED.delay(myDelay);
}
void scrollText(){
	if (ScrollingMsg.UpdateText() == -1) { // if end of text
		int Size = 0;
		while (TxtAncs[Size] != '\0') Size++;
		ScrollingMsg.SetText((unsigned char *)TxtAncs,Size);
	}
	else
	FastLED.show();
}
void randShapes(){
	int myDelay = map(variableDelay,0,256,(standardDelay*6),(standardDelay*40));
	int pos1,pos2,pos3,pos4,posR;
	static int shapeNumber;
	fadeToBlackBy(&(matrix(0)), NUM_LEDS,40);
	shapeNumber=random8(1,3);//square or circle +1 cus ? but works.
	colorIndex=random8(1,255);//color of shape
	pos1=random8(0,MATRIX_WIDTH);//starting x coord
	pos2=random8(0,MATRIX_HEIGHT);//starting y coord
	pos3=random8(0,MATRIX_WIDTH);//ending x coord
	pos4=random8(0,MATRIX_HEIGHT);//ending y coord
	posR=random8(1,(MATRIX_HEIGHT/2));//radius for circle
	if (shapeNumber==1){//purpose i want there to be at least 1 line of blank pixles in each rectangle
	//draw, so i check if the dfference in coordinates allows this, and if not edit to do so.
		if (pos1>=pos3){
			if((pos1-pos3)<2){
				if ((pos1-pos3)==1){
					pos1=pos1+1;
				}
				else{
					pos1=pos1+2;
				}	
			}
		}
		if (pos3>=pos1){
			if((pos3-pos1)<2){
				if ((pos3-pos1)==1){
					pos3=pos3+1;
				}
				else{
					pos3=pos3+2;
				}	
			}
		}
		if (pos2>=pos4){
			if((pos2-pos4)<2){
				if ((pos2-pos4)==1){
					pos2=pos2+1;
				}
				else{
					pos2=pos2+2;
				}	
			}
		}
		if (pos4>=pos2){
			if((pos4-pos2)<2){
				if ((pos4-pos2)==1){
					pos4=pos4+1;
				}
				else{
					pos4=pos4+2;
				}	
			}
		}
	}		
	switch (shapeNumber){//draw the shape.
		case 1:
			if (paletteMode==1){
				matrix.DrawRectangle(pos1,pos2,pos3,pos4,ColorFromPalette(currentPalette,colorIndex,brightness,currentBlending));
				break;
			}
			else{
				matrix.DrawRectangle(pos1,pos2,pos3,pos4,ourCol);
				break;
			}
		case 2:
			if (paletteMode==1){
				matrix.DrawCircle(pos1,pos2,posR,ColorFromPalette(currentPalette,colorIndex,brightness,currentBlending));  
				break;
			}
			else{
				matrix.DrawCircle(pos1,pos2,posR,ourCol); 
				break;
			}
	} 
	FastLED.delay(myDelay);
}
void expandingShape(){
	static int step,lastStep,R;
	static int shapeNumber=0;
	static int X1=(MATRIX_WIDTH/2);
	static int Y1=(MATRIX_HEIGHT/2);
	static int X2=(MATRIX_WIDTH/2);
	static int Y2=(MATRIX_HEIGHT/2);
	int xStart=(MATRIX_WIDTH/2);
	int	yStart=(MATRIX_HEIGHT/2);
	int myDelay = map(variableDelay,0,256,(standardDelay/20),(standardDelay*8));	
	step++;
	//shapeNumber=random8(0,2);//square or circle +1 cus ? but works.
	colorIndex=colorIndex+50;
	if (colorIndex>255){
		colorIndex=1;
	}
	if (step!=lastStep){
		if (X1<1){
			X1=(MATRIX_WIDTH/2);
			Y1=(MATRIX_HEIGHT/2);
			X2=(MATRIX_WIDTH/2);
			Y2=(MATRIX_HEIGHT/2);
			shapeNumber=random8(0,2);
		}
		else{
			X1=X1-1;
			Y1=Y1-1;
			X2=X2+1;
			Y2=Y2+1;
		}
		if (R<(MATRIX_WIDTH/2)){
			R=R+1;
		}
		else{
			R=1;
			shapeNumber=random8(0,2);
		}
		if (paletteMode==1){
			if(shapeNumber==0){
				matrix.DrawRectangle(X1,Y1,X2,Y2,ColorFromPalette(currentPalette,colorIndex,brightness,currentBlending));
			}
			else{
				matrix.DrawCircle(xStart,yStart,R,ColorFromPalette(currentPalette,colorIndex,brightness,currentBlending));
			}
		}
		else{
			fadeToBlackBy(&(matrix(0)), NUM_LEDS,40);
			if(shapeNumber==0){
				matrix.DrawRectangle(X1,Y1,X2,Y2,ourCol);
			}
			else{
				matrix.DrawCircle(xStart,yStart,R,ourCol);
			}
		}
		step=lastStep;
	}
	FastLED.delay(myDelay);
}
void retractingShape(){
	static int step,lastStep,R;
	static int shapeNumber=0;
	static int X1=1;
	static int Y1=1;
	static int X2=MATRIX_WIDTH;
	static int Y2=MATRIX_HEIGHT;
	int xStart=(MATRIX_WIDTH/2);
	int	yStart=(MATRIX_HEIGHT/2);
	int myDelay = map(variableDelay,0,256,(standardDelay),(standardDelay*8));	
	step++;
	//shapeNumber=random8(0,2);//square or circle +1 cus ? but works.
	colorIndex=colorIndex+40;
	if (colorIndex>255){
		colorIndex=1;
	}	
	if (step!=lastStep){
		if (X1>(MATRIX_WIDTH/2)){
			X1=0;
			Y1=0;
			X2=MATRIX_WIDTH;
			Y2=MATRIX_HEIGHT;
			//shapeNumber=random8(0,2);
		}
		else{
			X1++;
			Y1++;
			X2--;
			Y2--;
		}
		if (R<(MATRIX_WIDTH/2)){
			R=R+1;
		}
		else{
			R=1;
			//shapeNumber=random8(0,2);
		}
		if (paletteMode==1){
			if(shapeNumber==0){
				matrix.DrawRectangle(X1,Y1,X2,Y2,ColorFromPalette(currentPalette,colorIndex,brightness,currentBlending));
			}
			else{
				matrix.DrawCircle(xStart,yStart,R,ColorFromPalette(currentPalette,colorIndex,brightness,currentBlending));
			}
		}
		else{
			fadeToBlackBy(&(matrix(0)), NUM_LEDS,80);
			if(shapeNumber==0){
				matrix.DrawRectangle(X1,Y1,X2,Y2,ourCol);
			}
			else{
				matrix.DrawCircle(xStart,yStart,R,ourCol);
			}
		}
		step=lastStep;
	}
	FastLED.delay(myDelay);
}
///////////////////////////end of patterns///////////////////////
void paletteSelect(){
	switch (paletteNumber){
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
void patternSelect(){
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
			simpleStrobe();
			break;
		case 9:
			bouncingTrails();
			break;
		case 10:
			scrollText();
			break;
		case 11:
			randShapes();
			break;
		case 12:
			expandingShape();
			break;
		case 13:
			retractingShape();
			break;
    }
}
void mirrorSelect(){
	switch(mirrorNumber){
		case 0:
			break;
		case 1:
			matrix.QuadrantMirror();
			break;
		case 2:
			matrix.QuadrantRotateMirror();
			break;
		case 3:
			matrix.VerticalMirror();
			break;
	}
}
void loop(){
	EVERY_N_SECONDS(1) {      
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
	}
	patternNumber=13;
	paletteSelect();
	patternSelect();
	mirrorSelect();
}
