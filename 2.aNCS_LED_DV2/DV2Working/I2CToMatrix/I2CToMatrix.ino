#include <FastLED.h>
#include <Wire.h>
#include <LEDMatrix.h>
#include <LEDText.h>
#include <FontMatrise.h>
// CHANGE THIS FOR ALTERNATE MATRIX
#define I2C_ADDR 3

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

cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;
cLEDText ScrollingMsg;
CRGB tst[256];
CRGB ourCol = CRGB(255, 255, 255);
char TxtAncs[160] = {" ANCS - CUSTOM BUILT AUDIO / VISUAL - WWW.ANCS.GQ    "};
int iic = 0; // holder for i2c message
String string = "";
int timeSinceBt = 0;
int autoMode = 1;
int autoSecs = 10;  
         
void setup() {
  Wire.begin(I2C_ADDR); //2,3,4 for A,B+C aNCS_2560 boards.
  Wire.onReceive(receiveEvent);
  DBEGIN(115200);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds[0], leds.Size());
  FastLED.setBrightness(64); // Limit Power Consumption 
  FastLED.clear(true);
  ScrollingMsg.SetFont(MatriseFontData); // or Robotron.
  ScrollingMsg.Init(&leds, leds.Width(), ScrollingMsg.FontHeight() + 1, 0, 0);
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
  byte randTrigger=0;//initializes a randX trigger (didn't need to be global);
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
    if (subs.startsWith("Ptn")) { // next value is pattern.
		DPRINT("PTN ");
		String ptn = string.substring(0, string.indexOf(",")); // get everything until the comma
		DPRINT(ptn);
		DPRINT(" - ");
		x = ptn.toInt(); // Its going to be an integer. its the pattern number,
		DPRINTLN(x);
		string.remove(0, string.indexOf(",") + 1); // Remove the value
		if(individualStripMode==1){
			patternStore[stripNumber]=x;
		}
    }
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
		String pno= string.substring(0, string.indexOf(",")); // get 	everything until the comma
		DPRINT(pno);
		DPRINT(" - ");
		paletteNumber = pno.toInt(); // Its going to be an integer. its the palette number.
		DPRINTLN(paletteNumber);
		string.remove(0, string.indexOf(",") + 1); // Remove the value
    }
    if (subs.startsWith("Ran")) { // next value will trigger randX
		DPRINT("Ran ");
		String ran= string.substring(0, string.indexOf(",")); // get 	everything until the comma
		DPRINT(ran);
		DPRINT(" - ");
		randTrigger = ran.toInt(); // Its going to be an integer. its the 	palette number.
		DPRINTLN(randTrigger);
		string.remove(0, string.indexOf(",") + 1); // Remove the value
		if (randTrigger!=0){
			randX();
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
	DPRINTLN(string.length()); // prints the length of the command each iteration  
	DPRINT("STR = "); // prints after length < 1
	DPRINTLN(string);
	string = ""; // empty it
  }
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
