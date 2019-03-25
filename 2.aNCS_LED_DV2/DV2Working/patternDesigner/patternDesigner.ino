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

cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> matrix;
cLEDText ScrollingMsg;
CRGB leds[256];
CRGB ourCol=(256,256,256);

CRGBPalette16 currentPalette;//holds the palette
TBlendType currentBlending;//blending type 
int colorIndex = 0;//holds the position in the palette array for the color to show.
uint8_t brightness = 255;
uint8_t hue;
int16_t counter;
int paletteMode=1;

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

void setup() {

	DBEGIN(115200);
	FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(matrix[0], matrix.Size());
	FastLED.setBrightness(64); // Limit Power Consumption 
	currentBlending = LINEARBLEND;
  currentPalette=Pastel1_08_gp;
	FastLED.clear(true);
	FastLED.showColor(CRGB::Red);
	delay(500);
	FastLED.showColor(CRGB::Green);
	delay(500);
	FastLED.showColor(CRGB::Blue);
	delay(500);
	FastLED.show();
	DPRINTLN("SETUP");

	hue-0;
	counter=0;
}
void setLEDs(){
  FL(0,256){
    matrix(i)=leds[i];
  }
}
void pattern(){
	int16_t sx, sy, x, y;
	uint8_t h;

	FastLED.clear();
  
	h = hue;

		// ** Fill LED's with diagonal stripes
		for (x=0; x<(matrix.Width()+matrix.Height()); ++x){
			matrix.DrawLine(x - matrix.Height(), matrix.Height() - 1, x, 0, CHSV(h, 255, 255));
			h+=16;
		}
	

	hue+=4;
	counter++;
	if (counter >= 2250)
		counter = 0;
    matrix.VerticalMirror();
		FastLED.show();
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
  
  counter++;
  if (counter==(MATRIX_WIDTH*4)){//used a multiple of 2 so that a new point s formed when the two trails are close to each other (looks better in my opinon)
    counter=0;
  }
  if (lastCount!=counter){//if the counter has progressed
    fadeToBlackBy(&(leds[0]), NUM_LEDS, 60);
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
        leds[(pos+(i*MATRIX_WIDTH))]=ColorFromPalette( currentPalette, paletteRef, brightness, currentBlending);
      }
      else{
        leds[(pos+(i*MATRIX_WIDTH))]=ourCol;
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
        leds[(posUp+(i*MATRIX_WIDTH))]=ColorFromPalette( currentPalette, paletteRef,  brightness, currentBlending);
        leds[(posDown+(i*MATRIX_WIDTH))]=ColorFromPalette( currentPalette, paletteRef, brightness, currentBlending);
      }
      else{
        leds[(posUp+(i*MATRIX_WIDTH))]=ourCol;
        leds[(posDown+(i*MATRIX_WIDTH))]=ourCol;
      }
    }
  }
  lastCount=counter;
}
void loop(){
  //pattern();
  bouncingTrails();
  //matrix.VerticalMirror();
  setLEDs();
  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}
