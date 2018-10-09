#include <pt.h> // ProtoThread Library
#include <clock.h> // Library for the CLOCK_SECOND constant
#include <timer.h> // Libery for the timer
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>  
// Struct that keep a protothread instance
static struct pt ptLed; // instance for the LED protothread
static struct pt ptEth; // instance for the serial protothread
/* FastLED setup */
#include "FastLED.h"
#define DATA_PIN    46
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    29
#define BRIGHTNESS          64
#define FRAMES_PER_SECOND  120
#define UDP_TX_PACKET_MAX_SIZE 64
uint8_t gHue = 0;
CRGB leds[NUM_LEDS];
int ColorSel = 4;
int lastSel = 0;
int ptrnSel = 1;
CRGB ledColorSelections[8]={CRGB::Black,CRGB::Yellow,CRGB::Magenta,CRGB::Green,CRGB::Red,CRGB::Blue,CRGB::Cyan,CRGB::White};
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);

unsigned int localPort = 8888;      // local port to listen on

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  //buffer to hold incoming packet,
char  ReplyBuffer[] = "acknowledged";       // a string to send back
String UdpMsgArray[20];
// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;
void setup(){
  // Init both instances
  PT_INIT(&ptLed); //Leds thread
PT_INIT(&ptEth); //Touch thread
   // start the Ethernet and UDP:
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(9600);
  Serial.println("Setup Done!");
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
    if (ColorSel != lastSel) {
        lastSel = ColorSel;
      Serial.print("ColorSel is - ");Serial.print(ColorSel);
      }
    if (ptrnSel == 0){
      setAll(CRGB(ledColorSelections[ColorSel]));
    }
    if (ptrnSel == 1) {
      sinelon(CRGB(ledColorSelections[ColorSel]));
    }
      timer_reset(&t);
      FastLED.show();

       }
  PT_END(pt); // End of the protothread
}

static PT_THREAD(runEth(struct pt *pt)){
   static struct timer t; // timer of protothread
  PT_BEGIN(pt); // Begin the protothread
    timer_set(&t, 0.05*CLOCK_SECOND); // set the timer to expire
                                 // in 1 second
   
  while(1){
    PT_WAIT_UNTIL(pt, timer_expired(&t));
    // if there's data available, read a packet
    int packetSize = Udp.parsePacket();
    if (packetSize) {
      Serial.print("Received packet of size ");
      Serial.println(packetSize);
      Serial.print("From ");
      IPAddress remote = Udp.remoteIP();
      for (int i = 0; i < 4; i++) {
        Serial.print(remote[i], DEC);
          if (i < 3) {
            Serial.print(".");
          }
        }
        Serial.print(", port ");
        Serial.println(Udp.remotePort());

       // read the packet into packetBufffer
       Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
       Serial.println("Contents:");
       Serial.println(packetBuffer);
       String bufferRead = packetBuffer ;
       int pos = 0;
       int lengthofstring = bufferRead.length();
       int numVars =0; 
       while (pos< lengthofstring){
        Serial.println(bufferRead);
        int newPos = bufferRead.indexOf(',');
        UdpMsgArray[numVars] = bufferRead.substring(pos,newPos) ;
        numVars+=1;
        pos=newPos;
        bufferRead = bufferRead.substring(pos);
        lengthofstring = bufferRead.length();
        Serial.print(pos);Serial.print(" - ");Serial.println(lengthofstring);
           }
       // Serial.print("got ");Serial.print(numVars/2);Serial.println("channels and their floats");
      for (int i=0; i<numVars; i+2){
        Serial.print(UdpMsgArray[i]);Serial.print(" - ");Serial.println(UdpMsgArray[i+1]);
      if (UdpMsgArray[i] == "/Color"){
      float theVar = UdpMsgArray[i+1].toFloat();
      ColorSel = map(theVar,0,1,0,8) ;
      Serial.println("got Color instruction");
      }
      if (UdpMsgArray[i] == "/Pattern"){
        float theVar = UdpMsgArray[i+1].toFloat();
        ptrnSel = map(theVar,0,1,0,8);
       }
     }
   }
 timer_reset(&t);
  }
  
  PT_END(pt); // End of the protothread
}

void loop(){
  // The calls of both protothreads
  runLeds(&ptLed);
  runEth(&ptEth);
  
  // The call of protothread functions should be periodically
  // the system won't call it automatically
}
void setAll(CRGB color) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    leds[i] = color;
  }
}
void sinelon(CRGB color){
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] = color;
  leds[pos] += CHSV( gHue, 255, 192);
}
