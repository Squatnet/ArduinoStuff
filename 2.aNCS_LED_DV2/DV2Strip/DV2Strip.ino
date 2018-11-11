// PJON DEFINES //
#include <PJON.h>
#include "FastLED.h"
#define NUM_LEDS 29
#define DATA_PIN 2
#define DATA_PIN_2 3
#define DATA_PIN_3 4
#define DATA_PIN_4 5
#define FRAMES_PER_SECOND  120
#define CONNECTED_STRIPS 4
#define FL(aa,bb) for (int i = aa; i < bb; i++)
CRGB ledsA[NUM_LEDS];
CRGB ledsB[NUM_LEDS];
//CRGB ledsC[NUM_LEDS];
//CRGB ledsD[NUM_LEDS];
CRGB ourCol = CRGB(255, 255, 255);
CRGB startup[] = {CRGB(255, 123, 0), CRGB(0, 255, 45), CRGB(0, 123, 255), CRGB(0, 255, 255)};

// PJON DEFINES
#include <PJON.h>
#include<ArduinoJson.h>
// Bus id definition
uint8_t bus_id[] = {0, 0, 1, 53};
// PJON object
PJON<SoftwareBitBang> bus(bus_id, 2);
int packet;
bool acquired = false;


// internal vars
int x = 0; // holder for i2c message
String string = "";
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
int timeSinceBt = 0;
int autoMode = 1;
int autoSecs = 10;

void receiver_handler(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  const char * arr = payload;
  Serial.println();
  Serial.print("Got Message : ");
  Serial.println(arr);
  if(arr[0] == '{'){
    const size_t bufferSize = JSON_OBJECT_SIZE(4) + 60;
    DynamicJsonBuffer jsonBuffer(bufferSize);
    JsonObject& Msg = jsonBuffer.parseObject(arr);
    if(Msg.success()){
      Serial.println("Parse Success");
      //Msg.printTo(Serial);
      if(Msg.containsKey("pattern"))x = Msg["pattern"];
      if(Msg.containsKey("color"))ourCol = CRGB(Msg["color"].as<char*>());
      if(Msg.containsKey("timeout"))autoSecs = Msg["timeout"];
      if(Msg.containsKey("automode"))autoMode = Msg["automode"];
      if(Msg.containsKey("poll"))respondToPoll();
    }
    else Serial.println("PARSE FAIL");
  }
  else Serial.println(arr);
};
void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection with device ID ");
    Serial.print(bus.packets[data].content[0], DEC);
    Serial.println(" is lost.");
  }
  if(code == PJON_PACKETS_BUFFER_FULL) {
    Serial.print("Packet buffer is full, has now a length of ");
    Serial.println(data);
    Serial.println("Possible wrong bus configuration!");
    Serial.println("higher PJON_MAX_PACKETS if necessary.");
  }
  if(code == PJON_CONTENT_TOO_LONG) {
    Serial.print("Content is too long, length: ");
    Serial.println(data);
  }
};
void respondToPoll(){
  Serial.println("Responding to poll request");
  String msg = "{\"pr\":\" \",\"ptn\":\"";
  msg += String(x);
  msg += "\",\"clr\":[";
  msg += String(ourCol.r);
  msg += ",";
  msg += String(ourCol.g);
  msg += ",";
  msg += String(ourCol.b);
  msg += "],\"am\":";
  msg += String(autoMode);
  msg += ",\"as\":";
  msg += String(autoSecs);
  msg += "}";
  Serial.println(msg);
  char JSON[msg.length()+1];
  msg.toCharArray(JSON,msg.length()+1);
    int Size = 0;
    while (JSON[Size] != '\0') Size++;
    int err = bus.send_packet(1,JSON,Size);
    Serial.print(err);
}
void tellMasterAboutSelf(){
  const char * JSON = "{\"register\":\"strip\",\"name\":\"A\"}"; // enter the json to describe this slave here, note the \" to escape
    int Size = 0;
    while (JSON[Size] != '\0') Size++;
    if(bus.send_packet(1,JSON,Size) == PJON_ACK){
    Serial.println("Hi MOM");
    Serial.print(JSON);
   }
  else{
    Serial.println("failed");
  }
};
void setup() {
  Serial.begin(115200);
  Serial.println("Setup");
  bus.strategy.set_pin(12);
  bus.set_receiver(receiver_handler);
  bus.set_error(error_handler);
  bus.begin();
  tellMasterAboutSelf();
  acquired = true;
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(ledsA, NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN_2, GRB>(ledsB, NUM_LEDS);
  //FastLED.addLeds<WS2812B, DATA_PIN_3, GRB>(ledsC, NUM_LEDS);
  //FastLED.addLeds<WS2812B, DATA_PIN_4, GRB>(ledsD, NUM_LEDS);
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
  randX();
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
    //ledsC[i] = ledsA[i];
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
  //call function
  yield();
  copyLeds();
  bus.update();
  bus.receive(5000);
  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND); // insert a delay to keep the framerate modest

  //  EVERY_N_MILLISECONDS( 20 )
}
