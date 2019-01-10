#include <PJONSlave.h>

// PJON object
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include "Adafruit_GFX.h"// Hardware-specific library
#include <MCUFRIEND_kbv.h> // TFT Stuff

uint8_t bus_id[4] = {0,0,1,53}; // aNCS Unique Bus ID :)

PJONSlave<SoftwareBitBang> bus(bus_id, PJON_NOT_ASSIGNED);
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
void(* resetFunc) (void) = 0;
void progmemPrint(const char *str);
void progmemPrintln(const char *str);
int ourID = 255;
int packet;
char content[] = "01234567890123456789";
bool acquired = false;

void receiver_handler(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  Serial.print("Received: ");
  for(uint16_t i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.flush();
};

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection lost with device ");
    Serial.println((uint8_t)bus.packets[data].content[0], DEC);
    delay(1000);
    resetFunc();
  }
  if(code == PJON_ID_ACQUISITION_FAIL) {
    if(data == PJON_ID_ACQUIRE)
      Serial.println("PJONSlave error: multi-master addressing failed.");
    if(data == PJON_ID_CONFIRM)
      Serial.println("PJONSlave error: master-slave id confirmation failed.");
    if(data == PJON_ID_NEGATE)
      Serial.println("PJONSlave error: master-slave id release failed.");
    if(data == PJON_ID_REQUEST)
      Serial.println("PJONSlave error: master-slave id request failed.");
      delay(400);
      if (millis() > 15000){
        Serial.println("Resetting due to no ID");
        delay(300);
        resetFunc();
        }
      bus.acquire_id_master_slave();
      acquired = false;
      delay(160);
     
 }
  Serial.flush();
};

void setup() {
  Serial.begin(115200);
  bus.set_error(error_handler);
  Serial.println("Setup");
  delay(1060);
  bus.strategy.set_pin(12);
  bus.set_receiver(receiver_function);
  bus.acquire_id_master_slave();
  bus.begin();
  delay(160);
  Serial.print("Bus init ");
  Serial.println(bus.device_id());
  //bus.send(1, "B", 1);
 uint16_t ID = tft.readID();
    tft.begin(ID);
    menuScreen();
    
}
void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
     if(payload[0] != '\0'){
      uint8_t id = packet_info.sender_id;
      mScreen(payload,length,id);
     for(uint8_t i = 0; i < length; i++) {
      Serial.print(char( payload[i]));
     }
    Serial.println(" ");
    }
}
void loop() {
  if((bus.device_id() != PJON_NOT_ASSIGNED) && !acquired) {
    Serial.print("Acquired device id: ");
    Serial.println(bus.device_id());
    ourID = bus.device_id();
    Serial.flush();
    delay(100);
    doFooter();
    acquired = true;
    tellMasterAboutSelf();
  }
  bus.update();
  bus.receive(5000);
}
unsigned long mScreen(uint8_t *payload,uint16_t length,int sender) { // make this receive char* or string whatever :) have fun
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
void tellMasterAboutSelf(){
  String newPkt = "Reg,Term,MasterTerm";
  const char pkt[newPkt.length()+1];
  newPkt.toCharArray(pkt,newPkt.length());
  bus.send_packet(254,pkt,newPkt.length()+1);
}
unsigned long menuScreen() {
    unsigned long start;
    //SETUP
    tft.fillScreen(BLACK);
    start = micros();
    tft.setRotation(2); // 3 or 1 for Landscape mode, 2 / 4 for Vertical.
    tft.setCursor(0, 0);
    //HEADER
    doHeader();
    //MENU
    doFooter();
    

 tft.setCursor(0, 90);
    return micros() - start;
}
