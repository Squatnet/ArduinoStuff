#include <PJON.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include "Adafruit_GFX.h"// Hardware-specific library
#include <MCUFRIEND_kbv.h> // TFT Stuff

uint8_t bus_id[4] = {0,0,1,53}; // aNCS Unique Bus ID :)
PJON<SoftwareBitBang> bus(bus_id, 10); // Correct initializer.

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
MCUFRIEND_kbv tft; // assign tft mcufriend related stuff to an alias - tft
/**************Declare human readable colors *****************/
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

void progmemPrint(const char *str);
void progmemPrintln(const char *str);

void setup() {
  Serial.begin(9600);
  Serial.println("Setup");
  bus.strategy.set_pin(12);
  bus.set_receiver(receiver_function);
  bus.begin();
  //bus.send(1, "B", 1);
 uint16_t ID = tft.readID();
    tft.begin(ID);
    menuScreen();
    
}
void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if(payload[0] == 'A') {
   
    mScreen();
   
  }
  if(payload[0] == 'B') {
    
    mScreen();
   
  }
  if(payload[0] == 'C') {
    
    mScreen();
   
  }
  if(payload[0] == 'D') {
    
    mScreen();
   
  }
 
}
void loop() {
 bus.receive(1000);
  bus.update();
}
unsigned long mScreen() { // make this receive char* or string whatever :) have fun
    unsigned long start;

   
    tft.setTextColor(WHITE);
    tft.setTextSize(1); tft.print(" Insert String  ");
    
  return micros() - start;
}
unsigned long menuScreen() {
    unsigned long start;
    //SETUP
    tft.fillScreen(BLACK);
    start = micros();
    tft.setRotation(3); // 3 or 1 for Landscape mode, 2 / 4 for Vertical.
    tft.setCursor(0, 0);
    //HEADER
    tft.setTextColor(WHITE);
    tft.setTextSize(3); tft.println(" autoNoiseCreationStation");
    tft.setTextColor(MAGENTA);
    tft.setTextSize(2); tft.print("       DEV HUB #1 - ");
    tft.setTextColor(RED);
    tft.setTextSize(1); tft.println("Interactive Omnipresent Lighting Network ");
    //MENU
    
    //FOOTER
    tft.setCursor(0, 290);
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.println("Network status: Active - Input ID here");
    tft.setTextColor(GREEN);
    tft.println("PJON NETWORK TEST PLATFORM - DEMONSTRATION insert date and time ");
    tft.print("Last received Message information, timestamp");
    tft.setTextColor(RED);;
    tft.println("                              V0.1");
 tft.setCursor(0, 90);
    return micros() - start;
}

