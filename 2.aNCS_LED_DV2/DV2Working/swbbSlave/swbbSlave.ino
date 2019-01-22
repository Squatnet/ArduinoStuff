// FastLEDS //
#include "FastLED.h"
#define NUM_LEDS 28
#define DATA_PIN 2
#define DATA_PIN_2 3
#define DATA_PIN_3 4
#define DATA_PIN_4 5
#define FRAMES_PER_SECOND  120
#define CONNECTED_STRIPS 4
#define FL(aa,bb) for (int i = aa; i < bb; i++)
CRGB ledsA[NUM_LEDS];
CRGB ledsB[NUM_LEDS];
CRGB ledsC[NUM_LEDS];
CRGB ledsD[NUM_LEDS];
CRGB ourCol = CRGB(255, 255, 255);
CRGB startup[] = {CRGB(255, 123, 0), CRGB(0, 255, 45), CRGB(0, 123, 255), CRGB(0, 255, 255)};
// PJON //
#include <PJONSlave.h> 
uint8_t bus_id[] = {0, 0, 1, 53};
PJONSlave<SoftwareBitBang> bus(bus_id, PJON_NOT_ASSIGNED); // Force no id... 
int packet;
bool acquired = false; // did we get an address? 
bool debugMode = false;
uint32_t t_millis;
int ourID = 255;
// INTERNAL VARS //
int x = 0; // holder for i2c message
String string = "";
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
int timeSinceBt = 0;
int autoMode = 1;
int autoSecs = 10;
bool ack = false;

void(* resetFunc) (void) = 0;
void receiver_handler(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
   const char * arr = payload;
  string.concat(arr);
  if( string.startsWith("ack")){
    ack = true;
    string = "";
    Serial.print("Heard from server");
    
  }
  else parser();
  Serial.print("Received: ");
  for(uint16_t i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.flush();
  
};
void parser(){
  while(string.length() >= 1){
    String subs = string.substring(0,string.indexOf(","));
    string.remove(0,string.indexOf(0,string.indexOf(",")+1));
    if (subs.startsWith("Rst"))resetFunc();
    if (subs.startsWith("ptn")){
      String ptn = string.substring(0,string.indexOf(","));
      x = ptn.toInt();
      string.remove(0,string.indexOf(0,string.indexOf(",")+1));
    }
    if (subs.startsWith("atm")){
      String atm = string.substring(0,string.indexOf(","));
      autoMode = atm.toInt();
      string.remove(0,string.indexOf(0,string.indexOf(",")+1));
    }
    if (subs.startsWith("ats")){
      String ats = string.substring(0,string.indexOf(","));
      autoSecs = ats.toInt();
      string.remove(0,string.indexOf(0,string.indexOf(",")+1));
    }
    if (subs.startsWith("col")){
      String r = string.substring(0,string.indexOf(","));
      ourCol.r = r.toInt();
      string.remove(0,string.indexOf(0,string.indexOf(",")+1));
      String b = string.substring(0,string.indexOf(","));
      ourCol.b = b.toInt();
      string.remove(0,string.indexOf(0,string.indexOf(",")+1));
      String g = string.substring(0,string.indexOf(","));
      ourCol.g = g.toInt();
      string.remove(0,string.indexOf(0,string.indexOf(",")+1));
    }
    Serial.println(string.length());
  }
  Serial.print("STR = ");
  Serial.println(string);
  string = "";
}
void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection lost with device ");
    Serial.println((uint8_t)bus.packets[data].content[0], DEC);
    delay(1000);
    resetFunc();
  }
  if(code == PJON_ID_ACQUISITION_FAIL) {
    if(data == PJON_ID_ACQUIRE) {
      Serial.println("PJONSlave error: multi-master addressing failed.");
      delay(1000);
      resetFunc();
    }
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
  bus.set_receiver(receiver_handler);
  bus.strategy.set_pin(12);
  bus.begin();
  delay(160); // possibly not needed if master is online
  bus.acquire_id_master_slave(); //get an idFastLED.addLeds<WS2812B, DATA_PIN, GRB>(ledsA, NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN_2, GRB>(ledsB, NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN_3, GRB>(ledsC, NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN_4, GRB>(ledsD, NUM_LEDS);
  FastLED.setBrightness(128);
  int i = 0;
  while (i < 4) {
    ourCol = startup[i];
    turnOn();
    copyLeds();
    FastLED.show();
    FastLED.delay(300);
    i++;
  }
  Serial.println("Done!!!");
}
void tellMasterAboutSelf(){
  String newPkt = "Reg,Str,Any";
  const char pkt[newPkt.length()+1];
  newPkt.toCharArray(pkt,newPkt.length());
  bus.send(254,pkt,newPkt.length()+1);
}
void randX() {
  if (autoMode == 1) {
    x = random(2, 9);
  }
}

void copyLeds() {
  ///for (int i = 0; i < NUM_LEDS; i++) 
  FL(0,NUM_LEDS){
    ledsB[i] = ledsA[i];
    ledsC[i] = ledsA[i];
    ledsD[i] = ledsA[i];
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
void theLights() { //  speckles and strobes
  fadeToBlackBy( ledsA, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  ledsA[pos] = ourCol;
}
void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( ledsA, NUM_LEDS, gHue, 7);
}
void addGlitter( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    ledsA[ random16(NUM_LEDS) ] += CRGB::White;
  }
}
void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}
void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( ledsA, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  ledsA[pos] = ourCol;
  ledsA[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( ledsA, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS - 1 );
  ledsA[pos] = ourCol;
  ledsA[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  FL(0,NUM_LEDS) { //9948
    ledsA[i] = ourCol;
    ledsA[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( ledsA, NUM_LEDS, 20);
  byte dothue = 0;
  FL(0,8) {
    ledsA[beatsin16( i + 7, 0, NUM_LEDS - 1 )] |= ourCol;
    ledsA[beatsin16( i + 7, 0, NUM_LEDS - 1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
void loop() {
  if(ourID == 255 && millis() > 15000){
    Serial.println("NO ID AFTER 15000");
    resetFunc();
  }
  if((bus.device_id() != PJON_NOT_ASSIGNED) && !acquired) {
    Serial.print("Acquired device id: ");
    Serial.println(bus.device_id());
    Serial.flush();
    delay(100);
    acquired = true;
    tellMasterAboutSelf();
  }
  bus.update();
  bus.receive(5000);
  EVERY_N_MILLISECONDS(30) {
    gHue++;
  }
  EVERY_N_SECONDS(1) {
    timeSinceBt++;
    if (timeSinceBt == autoSecs) {
      timeSinceBt = 0;
      randX();
    }
  }
  switch (x) {
    case 0:
      turnOff();
      break;
    case 1:
      turnOn();
      break;
    case 2:
      theLights();
      break;
    case 3:
      rainbow();
      break;
    case 4:
      rainbowWithGlitter();
      break;
    case 5:
      confetti();
      break;
    case 6:
      sinelon();
      break;
    case 7:
      bpm();
      break;
    case 8:
      juggle();
      break;

  }
  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND); // insert a delay to keep the framerate modest

}
