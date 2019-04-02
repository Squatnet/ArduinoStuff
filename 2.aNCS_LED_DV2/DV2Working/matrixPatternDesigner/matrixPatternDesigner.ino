#include <FastLED.h>
#include <FastLED_GFX.h>
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
CRGB leds[NUM_LEDS];
CRGB ourCol = CRGB(255, 255, 255);
CRGBPalette16 currentPalette;//holds the palette
TBlendType currentBlending;//blending type 
int colorIndex = 0;//holds the position in the palette array for the color to show.
int brightness = 64;

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

void setup() {
	DBEGIN(115200);
  randomSeed(analogRead(0));
	FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(matrix[0], matrix.Size());
	FastLED.setBrightness(64); // Limit Power Consumption 
	FastLED.clear(true);
	FastLED.showColor(CRGB::Red);
	delay(500);
	FastLED.showColor(CRGB::Green);
	delay(500);
	FastLED.showColor(CRGB::Blue);
	delay(500);
	FastLED.show();
  currentPalette=saga_07_gp;
	DPRINTLN("SETUP");
}
void setLEDs(){
	FL(0,256){
		matrix(i)=leds[i];
	}
}
void turnOn() {// for each LED turn it to ourCol.
	FL(0, NUM_LEDS) {
		leds[i] = ourCol;
	}
}
void turnOff() {//for each LED turn off.
	FL(0, NUM_LEDS) {
		leds[i] = CRGB( 0, 0, 0);
	}
}

void loop(){
  static int pos1,pos2,pos3,pos4,posR,shapeNumber;
  int numberOfShapes = 4;
  EVERY_N_MILLISECONDS(1000){
    /*shapeNumber++;
    if(shapeNumber>numberOfShapes){
      shapeNumber=0;
    }*/
    shapeNumber=random8(1,3);
    colorIndex=random8(1,255);
    pos1=random8(1,MATRIX_WIDTH);
    pos2=random8(2,MATRIX_HEIGHT);
	  pos3=random8(1,MATRIX_WIDTH);
    pos4=random8(2,MATRIX_HEIGHT);
    posR=random8(1,(MATRIX_HEIGHT/2));
   
    DPRINTLN("RandomAF ");
    DPRINT(pos1);
    DPRINT(" : ");
    DPRINT(pos2);
    DPRINT(" : ");
    DPRINT(pos3);
    DPRINT(" : ");
    DPRINTLN(pos4);
    DPRINT("Shape = ");
    DPRINTLN(shapeNumber);
   /* if (shapeNumber==0){
        matrix.DrawFilledRectangle(pos1,pos2,pos3,pos4,ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending));
        DPRINTLN("Switch 0"); 
    }
    else*/ if (shapeNumber==1){
      matrix.DrawRectangle(pos1,pos2,pos3,pos4,ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending));
      DPRINTLN("Switch 1");
    }
    else if (shapeNumber==2){
      matrix.DrawCircle(pos2,pos2,posR,ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending));  
      DPRINTLN("Switch 2");
    }/*
    else if (shapeNumber==3){
      matrix.DrawFilledCircle(pos2,pos2,posR,ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending));  
      DPRINTLN("Switch 3");
    }*/
  }
  EVERY_N_SECONDS(15){
    turnOff();
    setLEDs();
  }
    

    FastLED.show();   
    FastLED.delay(20);
} 
