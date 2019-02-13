// Varidaic Debug Macro
#define DEBUG   //Comment this line to disable Debug output
#ifdef DEBUG    // Debug is on
  #define DPRINT(...)    Serial.print(__VA_ARGS__)     //Sends our arguments to DPRINT()
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //Sends our arguments to DPRINTLN()
  #define DFLUSH(...)    Serial.flush()
#else // Debug is off
  #define DPRINT(...)     //Nothing Happens
  #define DPRINTLN(...)   //Nothing Happens
  #define DFLUSH(...)
#endif // end macro
#define DEBUG_LED 13
#include "FastLED.h"
#include <Wire.h>
#define NUM_LEDS 28
#define DATA_PIN 2
#define DATA_PIN_2 3
#define DATA_PIN_3 4
//#define DATA_PIN_4 5
#define FRAMES_PER_SECOND  120
#define ZOOMING_BEATS_PER_MINUTE 122
#define STROBE_BEATS_PER_MINUTE 97.5
#define I2C_ADDR 1
#define CONNECTED_STRIPS 3
#define FL(aa,bb) for (int i = aa; i < bb; i++)
CRGB ledsA[NUM_LEDS];
CRGB ledsB[NUM_LEDS];
CRGB ledsC[NUM_LEDS];
CRGB ledsD[NUM_LEDS];
CRGB ourCol = CRGB(255, 255, 255);
CRGB startup[] = {CRGB(255, 123, 0), CRGB(0, 255, 45), CRGB(0, 123, 255), CRGB(0, 255, 255)};
int x = 0; // holder for i2c message
String string = "";
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
int timeSinceBt = 0;
int autoMode = 1;
int autoSecs = 2;
int a = 0;//counter for millis timer. used by pixleRef1.
bool debugLED = false;
unsigned long timer1=0;//timer used by pixleRef1.
unsigned long quarterSec=50;
CRGBArray <(NUM_LEDS/*CONNECTED_STRIPS*/)> leds;

void setup() {
//  Wire.begin(I2C_ADDR);
//  Wire.onReceive(receiveEvent);
  Serial.begin(115200);
  pinMode(DEBUG_LED, OUTPUT);
  Serial.println("Ready for i2c");
  timer1=millis();
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(ledsA, NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN_2, GRB>(ledsB, NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN_3, GRB>(ledsC, NUM_LEDS);
 // FastLED.addLeds<WS2812B, DATA_PIN_4, GRB>(ledsD, NUM_LEDS);
 FL(0,NUM_LEDS){
  leds[i]=&ledsA[i];
 }
/* FL(0,NUM_LEDS){
  leds[(i+(NUM_LEDS*2)+1)]=&ledsB[i];
 }
 FL(0,NUM_LEDS){
  leds[(i+(NUM_LEDS*3)+2)]=&ledsC[i];
 }*/

  FastLED.setBrightness(128);
  int i = 0;
  while (i < 3) {
    ourCol = startup[i];
    turnOn();
    copyLeds();
    FastLED.show();
    FastLED.delay(300);
    i++;
  }
  turnOff();
  copyLeds();
  FastLED.show();
}

void loop(){
  pixleRef1();
  //FastLED.show();
   // FastLED.delay(1000 / FRAMES_PER_SECOND); // insert a delay to keep the framerate modest
  
}

void pixleRef1(){
  
  if ((millis()-timer1)>=quarterSec){
    timer1+=quarterSec;
    a++;
    DPRINTLN(a);
    leds[a]=CRGB::Red;
    if (a!=0){
      leds[a-1]=CRGB::Black;
    }
    if (a>(NUM_LEDS/*CONNECTED_STRIP*/)){
      a=0;
    }
  }
    
}
  

void copyLeds() {
  ///for (int i = 0; i < NUM_LEDS; i++) 
  FL(0,NUM_LEDS){
    
    ledsB[i] = ledsA[i];
    ledsC[i] = ledsA[i];
    //ledsD[i] = ledsA[i];
  }
}
void turnOff() {
  FL(0,NUM_LEDS) {
    ledsA[i] = CRGB( 0, 0, 0);
  }
}
void turnOn() {
  FL(0,NUM_LEDS) {
    ledsA[i] = ourCol;
  }
}
