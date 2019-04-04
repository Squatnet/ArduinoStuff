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

void randShapes(){
	int pos1,pos2,pos3,pos4,posR;
	static int shapeNumber;
	fadeToBlackBy(&(matrix(0)), NUM_LEDS,10);
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
			DPRINTLN("Case 0 hit");
			matrix.DrawRectangle(pos1,pos2,pos3,pos4,ColorFromPalette(currentPalette,colorIndex,brightness,currentBlending));
			break;
		case 2:
			DPRINTLN("Case 1 hit");
			matrix.DrawCircle(pos1,pos2,posR,ColorFromPalette(currentPalette,colorIndex,brightness,currentBlending));  
			break;
	} 
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
		if(shapeNumber==0){
			matrix.DrawRectangle(X1,Y1,X2,Y2,ColorFromPalette(currentPalette,colorIndex,brightness,currentBlending));
		}
		else{
			matrix.DrawCircle(xStart,yStart,R,ColorFromPalette(currentPalette,colorIndex,brightness,currentBlending));
		}
		step=lastStep;
	}		
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
		if(shapeNumber==0){
			matrix.DrawRectangle(X1,Y1,X2,Y2,ColorFromPalette(currentPalette,colorIndex,brightness,currentBlending));
		}
		else{
			matrix.DrawCircle(xStart,yStart,R,ColorFromPalette(currentPalette,colorIndex,brightness,currentBlending));
		}
		step=lastStep;
	}	
}
void confetti(){
	// random colored speckles that blink in and fade smoothly
	fadeToBlackBy(&(leds[0]), NUM_LEDS, 10);
	int pos = random16(0, NUM_LEDS);
	if (paletteMode==1){
		colorIndex++;
		if(colorIndex>254){
			colorIndex=0;
		}
		leds[pos] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
	}
	else {
		leds[pos] = ourCol;
		leds[pos] += CHSV( colorIndex + random8(64), 200, 255);
	}
}
void loop(){
  //retractingShape();
  //expandingShape();
  //randShapes();
  confetti();
  FastLED.show();   
  FastLED.delay(100);
}
