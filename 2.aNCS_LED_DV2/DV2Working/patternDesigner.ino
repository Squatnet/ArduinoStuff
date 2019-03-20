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

CRGBPalette16 currentPalette;//holds the palette
TBlendType currentBlending;//blending type 
int colorIndex = 0;//holds the position in the palette array for the color to show.
uint8_t brightness = 255;
uint8_t hue;
int16_t counter;

void setup() {

	DBEGIN(115200);
	FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(matrix[0], matrix.Size());
	FastLED.setBrightness(64); // Limit Power Consumption 
	currentBlending = LINEARBLEND;
	FastLED.clear(true);
	FastLED.showColor(CRGB::Red);
	delay(500);
	FastLED.showColor(CRGB::Green);
	delay(500);
	FastLED.showColor(CRGB::Blue);
	delay(500);
	FastLED.show();
	DPRINTLN("SETUP");
	patternNumber=9;
	hue-0;
	counter=0;
}
void pattern(){
	int16_t sx, sy, x, y;
	uint8_t h;

	FastLED.clear();
  
	h = hue;
	if (counter < 1125){
		// ** Fill LED's with diagonal stripes
		for (x=0; x<(leds.Width()+leds.Height()); ++x){
			leds.DrawLine(x - leds.Height(), leds.Height() - 1, x, 0, CHSV(h, 255, 255));
			h+=16;
		}
	}
	else{
		// ** Fill LED's with horizontal stripes
		for (y=0; y<leds.Height(); ++y){
			leds.DrawLine(0, y, leds.Width() - 1, y, CHSV(h, 255, 255));
			h+=16;
		}
	}
	hue+=4;
	counter++;
	if (counter >= 2250)
		counter = 0;
		FastLED.show();
}