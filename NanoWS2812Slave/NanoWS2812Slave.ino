#include "FastLED.h"
#include <Wire.h>
#define LED_PIN        6
#define COLOR_ORDER    GRB
#define CHIPSET        WS2812B
#define NUM_LEDS 30
#define DATA_PIN 6
#define FRAMES_PER_SECOND  120

CRGB leds[NUM_LEDS];

int x = 0; // holder for i2c message
int y = 0; // holder for numeric value
int z = 0; // holder for alphabetic character

void setup() { 
  Wire.begin(8);
  Wire.onReceive(receiveEvent); 
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(64);  	  
  }
void receiveEvent(int bytes) {
  x = Wire.read();    // read one character from the I2C
  if( isDigit(x) ) y = x;
  if( isAlpha(x) ) z = x;
}

void loop() { 
  switch (x) {
    case 0:
      return;
    case 1:
      theLights(); 
     break;
    case 'a':
      addRed();
      break;
  }
  //call function
  FastLED.show();  
  FastLED.delay(1000/FRAMES_PER_SECOND); // insert a delay to keep the framerate modest 
  
  //  EVERY_N_MILLISECONDS( 20 )
}  
  
void theLights() { //  speckles and strobes
  fadeToBlackBy( leds, 30, 10);
  int pos = random16(30);
  leds[pos] += CRGB::White;
}
void addRed(){
  for(int i = 0; i < NUM_LEDS; i++){
    leds[i] += CRGB( 20, 0, 0);
  }
}
void addGreen(){
  for(int i = 0; i < NUM_LEDS; i++){
    leds[i] += CRGB( 0, 20, 0);
  }
}
void addBlue(){
  for(int i = 0; i < NUM_LEDS; i++){
    leds[i] += CRGB( 0, 0, 20);
  }
}
