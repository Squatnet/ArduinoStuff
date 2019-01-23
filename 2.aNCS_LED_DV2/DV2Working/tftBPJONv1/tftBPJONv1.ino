// PJON stuff //
#define PJON_INCLUDE_SWBB
#include <PJONSlave.h>  // we are inslave mode .
uint8_t bus_id[] = {0, 0, 1, 53}; // Ancs unique bus ID
PJONSlave<SoftwareBitBang> bus(bus_id, PJON_NOT_ASSIGNED); // Force no id so master can assign us
int packet; // Holder for our packet
bool acquired = false; // did we get an address? 
bool debugMode = false; // Are we debugging
uint32_t t_millis; // tick tock
int ourID = 255; // Our ID number 

// TFT Stuff //
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include "Adafruit_GFX.h"// Hardware-specific library
#include <MCUFRIEND_kbv.h> // TFT Stuff
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
MCUFRIEND_kbv tft; // Creates the tft object
// Declare human readable colors for TFT //
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// define min if not already //
#ifndef min // if not already defined
#define min(a, b) (((a) < (b)) ? (a) : (b)) // this is how you work out min
#endif 

// Program vars //
void progmemPrint(const char *str); // Dont actually know what these 2 are for? 
void progmemPrintln(const char *str); // Rio shed any light? 
String string = ""; // hold incoming PJON message
bool ack = false; // Master has registered us?
void(* resetFunc) (void) = 0; // Software reset 

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) { // Master is gone !!
    Serial.print("Connection lost with device ");
    Serial.println((uint8_t)bus.packets[data].content[0], DEC);
    delay(1000); // wait a second
    resetFunc(); // Reset
  }
  if(code == PJON_ID_ACQUISITION_FAIL) { // Didnt get an addres... !
    if(data == PJON_ID_ACQUIRE) {
      Serial.println("PJONSlave error: multi-master addressing failed.");
      // Didnt get id in Multi-Master environment
      delay(1000);
      resetFunc(); // bye!
    }
    if(data == PJON_ID_CONFIRM) // failed to confirm id with master... This shouldnt happen
      Serial.println("PJONSlave error: master-slave id confirmation failed.");
    if(data == PJON_ID_NEGATE)
      // We wont encounter this as we dont intend to give up out ID
      Serial.println("PJONSlave error: master-slave id release failed.");
    if(data == PJON_ID_REQUEST)
      // We couldnt find a Master on the network.... 
      Serial.println("PJONSlave error: master-slave id request failed.");
      delay(400); // wait 400ms
      if (millis() > 15000){ // if 15s has passed
        Serial.println("Resetting due to no ID");
        delay(300); // we reset
        resetFunc();
        } // if not
      bus.acquire_id_master_slave(); // try and get an id again
      acquired = false; // make sure we resend our registration when we reconnect
      delay(160); // makes the delay about 500ms between retrys
     
 }
  Serial.flush(); // wait til serial is printed
};
// PJON Receiver function //
void receiver_handler(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  uint8_t id = packet_info.sender_id;
  const char * arr = payload; // Not a pointer now.... !
  string.concat(arr); // addd it to our string
  if( string.startsWith("ack")){ // master got our registation message!
    ack = true; // nice
    string = ""; // thats all it says!
    Serial.print("Heard from server");
    }
  if (string.startsWith("Rst")){ // Is it a reset message?
    resetFunc(); // Device resets
    }
  else{ 
    Serial.print(string); // Print the message to serial
    string = ""; // Empty the message
    mScreen(arr,length,id); // send arr to the TFT as its already char array (nice!)
    }
};

void setup() {
  Serial.begin(115200); // Serial console
  Serial.print("Setup ");
  bus.set_error(error_handler); // link PJON to error handler
  Serial.print(". ");
  bus.set_receiver(receiver_handler); // link PJON to receiver
  Serial.print(". ");
  bus.strategy.set_pin(12); // Set PJON pin
  Serial.print(". ");
  bus.begin(); // 
  Serial.print(". ");
  delay(160); // possibly not needed if master is online
  Serial.print(". ");
  bus.acquire_id_master_slave(); //get an id
  Serial.print(". ");
  Serial.println("Done!!!");
  uint16_t ID = tft.readID(); // read the TFT id 
  tft.begin(ID); // Begin connection with TFT
  menuScreen(); // Draw the initial Screen    
}
void loop() {
  if(ourID == 255 && millis() > 15000){ // No id has been assigned and 15s have elapsed
    Serial.println("NO ID AFTER 15000");
    resetFunc(); // reset
  }
  if((bus.device_id() != PJON_NOT_ASSIGNED) && !acquired) { // we have an id, but havent regisrtered
    Serial.print("Acquired device id: ");
    Serial.println(bus.device_id()); 
    ourID = bus.device_id(); // Set that shit
    Serial.flush(); // finish writing serial
    delay(100);
    doFooter(); // Redraw the footer to update the ID
    acquired = true; // track that
    tellMasterAboutSelf(); // and register
  }
  bus.update(); // update the PJON
  bus.receive(5000); // receive for a while
} //END
// prints a line to the TFT // 
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
// Draws the header //
void doHeader(){
    tft.setTextColor(WHITE);
    tft.setTextSize(2); tft.println(" autoNoiseCreationStation");
    tft.setTextColor(MAGENTA);
    tft.setTextSize(1); tft.print("DEV HUB #1 - ");
    tft.setTextColor(RED);
    tft.setTextSize(1); tft.println("Interactive Omnipresent Lighting Network ");
}
// Draws the footer //
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
// Registers with the master // 
void tellMasterAboutSelf(){
  String newPkt = "Reg,Term,MasterTerm";
  const char pkt[newPkt.length()+1];
  newPkt.toCharArray(pkt,newPkt.length());
  bus.send_packet(254,pkt,newPkt.length()+1);
}
// Draws the full screen //
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
