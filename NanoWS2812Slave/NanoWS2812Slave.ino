#include "FastLED.h"
#include <Wire.h>
#define COLOR_ORDER    GRB
#define CHIPSET        WS2812B
#define NUM_LEDS 30
#define DATA_PIN 6
#define FRAMES_PER_SECOND  120

CRGB leds[NUM_LEDS];

String ourString = "";
CRGB ourCol;
int x = 0; // holder for i2c message
int y = 0; // holder for numeric value
int z = 0; // holder for alphabetic character

void setup() { 
  Wire.begin(8);
  Wire.onReceive(receiveEvent); 
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(32); 
  ourCol = CRGB( 255, 255, 255); 	  
  }
void receiveEvent(int howMany) {
  Serial.print("i2c message ");
  if (Wire.available()) { // loop through all but the last
    int x = Wire.read(); // receive byte as a int
    Serial.println(x);
    while(Wire.available()){
      char c = Wire.read();
      ourString.concat(c);
    }
    Serial.println("STRING = "+ourString);
    char charbuf[8];
    ourString.toCharArray(charbuf,8);
    long int rgb=strtol(charbuf,0,16); //=>rgb=0x001234FE;
    byte r=(byte)(rgb>>16);
    byte g=(byte)(rgb>>8);
    byte b=(byte)(rgb);
    ourCol = CRGB(r,g,b);
    Serial.write(ourCol);
    Serial.println();
    y = 1;
  }

}

void loop() { 
  switch (x) {
    case 0:
      turnOff;
      break;
    case 1:
      theLights(); 
      break;
  }
  //call function
  FastLED.show();  
  FastLED.delay(1000/FRAMES_PER_SECOND); // insert a delay to keep the framerate modest 
  
  //  EVERY_N_MILLISECONDS( 20 )
}  
void turnOff() {
  for (int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB::Black;
  }
}
void theLights() { //  speckles and strobes
  fadeToBlackBy( leds, 30, 10);
  int pos = random16(30);
  leds[pos] += ourCol;
}
void requestEvent() {
  Wire.write(y); // respond with message of 6 bytes
  y = 0;
  // as expected by master
}
