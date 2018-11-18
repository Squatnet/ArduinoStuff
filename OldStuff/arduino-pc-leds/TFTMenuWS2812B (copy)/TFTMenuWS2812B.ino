#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include "Adafruit_GFX.h"// Hardware-specific library
#include <MCUFRIEND_kbv.h> // TFT Stuff
#include <TouchScreen.h> // Library for touchscreen
#include <SPI.h>          // f.k. for Arduino-1.5.2
#include "FastLED.h"    // FastLED library
/* PIN numbers for LCD */
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
MCUFRIEND_kbv tft; // assign tft mcufriend related stuff to an alias - tft
/* Define number of leds and gpio pin */ 
#define NUM_LEDS 29 // num leds
#define DATA_PIN 46 // data pin
CRGB leds[NUM_LEDS]; // setup array of leds
/**************Declare human readable colors *****************/
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

/* setup touchscreen */
#define YP A1   //A3 for ILI9320
#define YM 7    //9
#define XM A2
#define XP 6    //8  
TouchScreen menuOne(XP, YP, XM, YM, 300);
TSPoint tp; 
void readResistiveTouch(void)
{
    tp = menuOne.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);
    digitalWrite(XM, HIGH);
}

bool ISPRESSED(void)
{
    readResistiveTouch();
    return tp.z > 20 && tp.z < 1000;
}
//#include <UTFT.h>
//#include <UTouch.h>
//UTFT    myGLCD(ILI9486, 38, 39, 40, 41);
//UTouch  myTouch( 6, 5, 4, 3, 2);

#define ISPRESSED() menuOne.dataAvailable()
extern void fail();

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

void progmemPrint(const char *str);
void progmemPrintln(const char *str);


uint16_t g_identifier;

int menuSel;

void menuScr(){
  TSPoint p = menuOne.getPoint();
  Serial.println("ran menuScr");
  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  if (p.z > menuOne.pressureThreshhold) {
    Serial.println("DetectedTouch");
    if(p.x > 680 & p.x < 800){
        menuSel = 1;
        Serial.println(menuSel);
    }
    if(p.x > 540 & p.x < 660){
        menuSel = 2;
        Serial.println(menuSel);
    }
    if(p.x > 380 & p.x < 520){
        menuSel = 3;
        Serial.println(menuSel);
    }
    // Serial.print("X = "); Serial.print(p.x);
     //Serial.print("\tY = "); Serial.print(p.y);
     //Serial.print("\tPressure = "); Serial.println(p.z);
  }
}
void oneLEDw() {
   // Move a single white led 
   for(int whitLed = 0; whitLed < NUM_LEDS; whitLed = whitLed + 1) {
        leds[whitLed] = CRGB::Red;
        FastLED.show();
        delay(100);
        leds[whitLed] = CRGB::Black;
   }
}
void oneLEDg() {
   // Move a single white led 
   for(int whitLed = 0; whitLed < NUM_LEDS; whitLed = whitLed + 1) {
        leds[whitLed] = CRGB::Red;
        FastLED.show();
        delay(100);
        leds[whitLed] = CRGB::Black;
   }
}
void oneLEDr() {
   // Move a single white led 
   for(int whitLed = 0; whitLed < NUM_LEDS; whitLed = whitLed + 1) {
        leds[whitLed] = CRGB::Green;
        FastLED.show();
        delay(100);
        leds[whitLed] = CRGB::Black;
   }
}
void oneLEDy() {
   // Move a single white led 
   for(int whitLed = 0; whitLed < NUM_LEDS; whitLed = whitLed + 1) {
        leds[whitLed] = CRGB::Blue;
        FastLED.show();
        delay(100);
        leds[whitLed] = CRGB::Black;
   }
}

/******************** SETUP **********************************/
void setup(void) {
    Serial.begin(9600);
    Serial.println("SETUP RUN!");
    if (!Serial) delay(5000);
    uint16_t ID = tft.readID();
    tft.begin(ID);
    //FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    menuScreen();
}

/********************************************** LOOP ************************************************/
void loop(void) {
menuScr();
switch(menuSel){
  case 0:
    oneLEDw();
    menuScr();
  case 1:
    oneLEDg();
    menuScr();
    break;
  case 2:
    oneLEDr();
    menuScr();
    break;
  case 3:
    oneLEDy();
    menuScr();
    break;  
}
}

unsigned long menuScreen() {
    unsigned long start;
    //SETUP
    Serial.println("menuScreen Setuo ran!");
    tft.fillScreen(BLACK);
    start = micros();
    tft.setRotation(3); // 3 or 1 for Landscape mode, 2 / 4 for Vertical.
    tft.setCursor(0, 0);
    //HEADER
    tft.setTextColor(WHITE);
    tft.setTextSize(2); tft.println("autoNoiseCreationStation");
    tft.setTextColor(MAGENTA);
    tft.setTextSize(1); tft.print("W45T3G45H - ");
    tft.setTextColor(RED);
    tft.setTextSize(1); tft.println("LED Backlight Controller Project");
    //MENU
    tft.setTextSize(4); tft.println(); 
    tft.setTextColor(WHITE);
    tft.setTextSize(2); tft.print("Option 1 = "); tft.setTextColor(GREEN); tft.println(" Green ");
    tft.setTextSize(4); tft.println();
    tft.setTextColor(WHITE);
    tft.setTextSize(2); tft.print("Option 2 = "); tft.setTextColor(RED); tft.println(" RED ");
    tft.setTextSize(4); tft.println();
    tft.setTextColor(WHITE);
    tft.setTextSize(2); tft.print("Option 3 = "); tft.setTextColor(BLUE); tft.println("BLUE");
    tft.setTextSize(4); tft.println();
    //FOOTER
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.println("Current Controller Information");
    tft.setTextColor(GREEN);
    tft.println("fastLED enabled - TFT Setup complete ");
    tft.println("Current Setting = ##################");
    tft.setTextColor(RED);;
    tft.println("                                            POWER OFF");

    return micros() - start;
}

