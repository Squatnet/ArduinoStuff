#include <PJON.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include "Adafruit_GFX.h"// Hardware-specific library
#include <MCUFRIEND_kbv.h> // TFT Stuff

uint8_t bus_id[4] = {0,0,1,53}; // aNCS Unique Bus ID :)
uint8_t ourID = 10;
PJON<SoftwareBitBang> bus(bus_id, ourID); // Correct initializer.

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
MCUFRIEND_kbv tft; // assign tft mcufriend related stuff to an alias - tft
/**************Declare human readable colors *****************/
#define BLACK   0x0000
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
  Serial.println("Setup Start");
  bus.strategy.set_pin(12);
  bus.set_receiver(receiver_function);
  bus.begin();
  //bus.send(1, "B", 1);
  uint16_t ID = tft.readID();
    tft.begin(ID);
    menuScreen();
  Serial.println("Setup Complete");  
}
void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
     if(payload[0] != '\0'){ // if the payload isn't null
      uint8_t id = packet_info.sender_id; //prepare the packet ...
        txt(payload,length,id); // ... and send to the TFT
          /*
          for(uint8_t i = 0; i < length; i++) {
            Serial.print(char( payload[i]));
          }
    Serial.println(" ");
    */
    }
}

void loop() {
 bus.receive(5000);
  bus.update();
}

unsigned long txt(uint8_t *payload,uint16_t length,int sender) { // make this receive char* or string whatever :) have fun
    unsigned long start;
    tft.setTextColor(WHITE,BLACK);
    tft.setTextSize(1);
    String newline = "";
     for(uint8_t i = 0; i < length; i++) {
      tft.print(char( payload[i]));
    }
    tft.print(" sender: ");
    tft.println(sender);
  return micros() - start;
}

void doHeader(){
    tft.setTextColor(WHITE);
    tft.setTextSize(2); tft.println(" autoNoiseCreationStation");
    tft.setTextColor(MAGENTA);
    tft.setTextSize(1); tft.print("DEV HUB #1 - ");
    tft.setTextColor(RED);
    tft.setTextSize(1); tft.println("Interactive Omnipresent Lighting Network ");
}
void doFooter(){
      //FOOTER
    tft.setCursor(0, 464);
    tft.setTextColor(WHITE, BLACK);
    tft.setTextSize(1);
    tft.print("Network status: Active- ");
    tft.print(ourID);
    tft.println("     ");
    tft.setTextColor(GREEN);
    tft.print("PJON NETWORK TEST PLATFORM - DEMONSTRATION");
    tft.setTextColor(RED);;
    tft.println("       V0.1");
     tft.setCursor(0,50);
}
unsigned long menuScreen() {
    unsigned long start;
    tft.fillScreen(BLACK);
    start = micros();
    tft.setRotation(2); // 3 or 1 for Landscape mode, 2 / 4 for Vertical.
    tft.setCursor(0, 0);
    doHeader();
    doFooter();
    tft.setCursor(0, 90);
    return micros() - start;
}

