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
	 
	matrix.DrawFilledRectangle(5,1,10,4,ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending));
}
void loop(){
	EVERY_N_SECONDS(2){
		colorIndex++;
		if (colorIndex>255){
			colorIndex=0;
		}
	}
	pattern();
	//matrix.VerticalMirror();
	setLEDs();
	FastLED.show();
	FastLED.delay(1000 / FRAMES_PER_SECOND);
}
