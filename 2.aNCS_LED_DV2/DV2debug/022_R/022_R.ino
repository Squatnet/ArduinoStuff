// 022_R Router With PJON on 12 and 7, 3 analog pots and 1x 74hc595 LED array.

// Aim : to Receive pulse signals and relay messages to "branch" of lights and relay board
#include <PJONInteractiveRouter.h>

StrategyLink<SoftwareBitBang> link1;
StrategyLink<SoftwareBitBang> link2;

PJONAny bus1(&link1, (const uint8_t[4]){0,0,1,53});
PJONAny bus2(&link2, (const uint8_t[4]){0,0,0,2});

PJONInteractiveRouter<PJONSwitch> router(2, (PJONAny*[2]) {&bus1, &bus2});

#define LPIN 4 // Latch
#define CPIN 5 // Clock
#define DPIN 3 // Data

int aPins[] = {0,1,2};
int val[] = {0,0,0};

byte data; // 595 Byte data interger (Bits)
byte dataArray[11]; //LED Array Type 1

void aR(int bb){
  val[bb] = analogRead(bb);
  val[bb] = map(val[bb], 0, 1023, 0, 255);
  Serial.println(val[bb]);
}

void hc595(int aa) {
  data = dataArray[aa];
        digitalWrite(LPIN, 0);
        shiftOut(DPIN, CPIN, data);
        digitalWrite(LPIN, 1);
}
void setup() {
  link1.strategy.set_pin(7);
  link2.strategy.set_pin(12);
  router.set_receiver(receiver_function);
  router.set_router(true); // Pick up every packet passing
  router.begin();
  
  pinMode(LPIN, OUTPUT); // Init 595 (menu)
  dataArray[0] = 0b00000000;
  dataArray[1] = 0b11111100;
  dataArray[2] = 0b11110011;
  dataArray[3] = 0b00100000;
  dataArray[4] = 0b00010000;
  dataArray[5] = 0b00001000;
  dataArray[6] = 0b00000100;
  dataArray[7] = 0b00000010;
  dataArray[8] = 0b00000001;
  dataArray[9] = 0b11111111;
  dataArray[10] = 0b10101010;
}
void receiver_handler(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  const char * arr = payload;
  Serial.println(arr);
  timeSinceBt = 0;
  String str = arr;
  Serial.println(str);
  str.trim();
  String strB = string.substring(0, 1);
  str.remove(0, 1);
  if (strB == "X") {
    hc595(2);
  }
  else if (strB == "S") {
    hc595(2);
  }
  /*
  else {
    x = strB.toInt(); // pattern choice 
    if (str.length() == 6) {
      char charbuf[8];
      str.toCharArray(charbuf, 8);
      long int rgb = strtol(charbuf, 0, 16); //=>rgb=0x001234FE;
      byte r = (byte)(rgb >> 16);
      byte g = (byte)(rgb >> 8);
      byte b = (byte)(rgb);
      ourCol = CRGB(r, g, b); // set the colour value!
    }
  }
  */
  str = "";
}
void loop() {
 router.loop();
}

void shiftOut(int myDataPin, int myClockPin, byte myDataOut) {
  int i=0;  //internal function setup
  int pinState;
  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT); 
  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);
  for (i=7; i>=0; i--)  {
    digitalWrite(myClockPin, 0);
    if ( myDataOut & (1<<i) ) {
      pinState= 1;
    }
    else {  
      pinState= 0;
    }
    digitalWrite(myDataPin, pinState);
    digitalWrite(myClockPin, 1);
    digitalWrite(myDataPin, 0);
     }
  digitalWrite(myClockPin, 0); 
}
