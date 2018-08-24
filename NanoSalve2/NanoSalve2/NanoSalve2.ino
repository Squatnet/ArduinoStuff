
/* ProtoThread libs */
#include <pt.h> // ProtoThread Library
#include <clock.h> // Library for the CLOCK_SECOND constant
#include <timer.h> // Library for the timer
// Struct that keep a protothread instance
static struct pt ptLed; // instance for the LED protothread
static struct pt ptSerial; // instance for the serial protothread

#define FASTLED_ALLOW_INTERRUPTS 0
#include "FastLED.h"
#include <Wire.h>
#define COLOR_ORDER    GRB
#define CHIPSET        WS2812B
#define NUM_LEDS 30
#define DATA_PIN 6
#define FRAMES_PER_SECOND  120

CRGB leds[NUM_LEDS];

String ourString = "";
String cunt= "";
CRGB ourCol;
int x = 0; // holder for i2c message
int y = 0; // holder for numeric value
int z = 0; // holder for alphabetic character

void setup() { 
   // Init both "threads" 
  Serial.begin(9600);
  PT_INIT(&ptLed); //Leds thread
  PT_INIT(&ptSerial); //Touch thread
  Wire.begin(8);
  Wire.onReceive(receiveEvent); 
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(32); 
  ourCol = CRGB( 255, 255, 255); 	  
  }
void receiveEvent(int howMany) {
  Serial.print("i2c message ");
    while(Wire.available()){
      char c = Wire.read();
      ourString.concat(c);
    }
    ourString.trim();
    String ss = ourString.substring(0,1);
    ourString.remove(0,1);
    x = ss.toInt();
    Serial.print("INT = ");
    Serial.print(x);
    Serial.print(" STRING = ");
    Serial.print(ourString);
    char charbuf[8];
    ourString.toCharArray(charbuf,8);
    long int rgb=strtol(charbuf,0,16); //=>rgb=0x001234FE;
    byte r=(byte)(rgb>>16);
    byte g=(byte)(rgb>>8);
    byte b=(byte)(rgb);
    ourCol = CRGB(r,g,b);
    Serial.print(ourCol);
    Serial.println();
    y = 1;
  }
static PT_THREAD(runLeds(struct pt *pt)){
  static struct timer t; // timer of protothread
  PT_BEGIN(pt);
  timer_set(&t, 0.05*CLOCK_SECOND); // set the timer to expire
                                 // in 1 second
  
  // while loop, like loop() of arduino
  while(1){
    // wait until the timer expire
    PT_WAIT_UNTIL(pt, timer_expired(&t));
  }
  //call function
      switch (x) {
    case 0:
      turnOff;
      FastLED.show();
    case 1:
      theLights(); 
      FastLED.show();
      }
  FastLED.delay(1000/FRAMES_PER_SECOND); // insert a delay to keep the framerate modest 
  Serial.print("thread 1 ");
  timer_reset(&t);
  PT_END(pt); // End of the protothread
}

/* Thread for Touch input */
static PT_THREAD(getInput(struct pt *pt)){
  static struct timer t; // Timer for the delayed serial
  static char c; // variable to hold the char value
                 // Pay attention to the static keyword,
                 // this keyword will prevent the data loss
                 // after the protothred is yield.
                 // Any data that has to be keeped should have
                 // static keyword
  PT_BEGIN(pt); // Begin the protothread
  while(1){
    // wait 250ms before continue the code
    timer_set(&t, 0.3*CLOCK_SECOND);
    PT_WAIT_UNTIL(pt, timer_expired(&t));
  }
  Serial.print("thread 2");
  timer_reset(&t);
  PT_END(pt); // End of the protothread
}

/* Main loop */
void loop(){
  // The calls both protothreads
  runLeds(&ptLed);
  getInput(&ptSerial);
  
} 
void turnOff() {
  for (int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB::Black;
  }
}
void theLights() { //  speckles and strobes
  fadeToBlackBy( leds, 30, 10);
  int pos = random16(30);
  leds[pos] = ourCol;
}
void requestEvent() {
  Wire.write(y); // respond with message of 6 bytes
  y = 0;
  // as expected by master
}

