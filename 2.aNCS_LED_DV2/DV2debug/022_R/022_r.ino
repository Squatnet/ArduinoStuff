// 022_R Router With PJON on 12 and 7, 3 analog pots and 1x 74hc595 LED array.
// Aim : to Receive pulse signals and relay messages to "branch" of lights and relay board

//breakdown code for debug
#define fL(x,a,b) for(int x=a;x<b;x++)
#include <PJON.h>
uint8_t bus_id[4] = {0,0,1,53};
// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> bus(bus_id, 30); // 0,0,1,53::30
  //PJON<SoftwareBitBang> bus2(1); // 0,0,0,1::1
#define LPIN 3 // Latch
#define CPIN 4 // Clock
#define DPIN 2 // Data
int aPins[] = {0,1,2}; //analog pins
int val[] = {0,0,0}; // holder for analog values
int valM[] = {0,0,0};
byte data; // 595 Byte data interger (Bits)
byte dataArray[15]; //LED Array Type 1
String string = "";
String fStr = "";
char aStr[30];
int mSel = 0;
int ptrn = 0;
int pulse = 0;
int r = 0;
int g = 0;
int b = 0;


void aR(int bb){
  val[bb] = map(analogRead(bb), 0, 1023, 0, 255); // if menu.x==# map to x values
  if(valM[bb] > (val[bb] + 2) | valM[bb] < (val[bb] - 2 )) { //if memory if greater than or less than value +- a little bit to allow a buffer for loose readings
    if(mSel==0) {
      if(bb==0){      string = "Red";      }
      if(bb==1){      string = "Green";    }
      if(bb==2){      string = "Blue";     }  
    }
    if(mSel==1) {
      if(bb==0){      string = "Delay";      }
      if(bb==1){      string = "Shape";    }
      if(bb==2){      string = "FPS";     }  
    }
    fStr = String( "K-" + bb + string + val[bb]);
    fStr.toCharArray(aStr, sizeof(aStr)); // change to char array
    bus.send_packet(10, bus_id, aStr, sizeof(aStr)); // send packet to TFT (and send relevant info to master later)
    valM[bb] = val[bb]; // make memory the new value.
  }
}
void hc595(int aa) {
  data = dataArray[aa];
        digitalWrite(LPIN, 0);
        shiftOut(DPIN, CPIN, data);
        digitalWrite(LPIN, 1);
}

void setup() {
  pinMode(LPIN, OUTPUT); // Init 595 (menu)
  dataArray[0] = 0b00000000;
  dataArray[1] = 0b10000000;
  dataArray[2] = 0b01000000;
  dataArray[3] = 0b00100000;
  dataArray[4] = 0b00010000;
  dataArray[5] = 0b00001000;
  dataArray[6] = 0b00000100;
  dataArray[7] = 0b00000010;
  dataArray[8] = 0b00000001;
  dataArray[9] = 0b11111111;
  dataArray[10] = 0b10101010;
  dataArray[11] = 0b10000001;
  dataArray[12] = 0b01000010;
  dataArray[13] = 0b00100100;
  dataArray[14] = 0b00011000;
  // Init PJON
//  bus.set_error(error_handler);
    //bus2.set_error(error_handler);
  bus.set_receiver(receiver_function);
    //bus2.set_receiver(receiver_function_2);
  bus.strategy.set_pin(12); // main bus
    //bus.strategy.set_pins(7); // sub bus
  bus.begin();
    //bus2.begin();
  hc595(11);
  delay(100);
  hc595(12);
  delay(100);
  hc595(13);
  delay(100);
  hc595(14);
  delay(100);
  hc595(0);
}
/*
void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection with device ID ");
    Serial.print(bus.packets[data].content[0], DEC);
    Serial.println(" is lost.");
  }
  if(code == PJON_PACKETS_BUFFER_FULL) {
    Serial.print("Packet buffer is full, has now a length of ");
    Serial.println(data, DEC);
    Serial.println("Possible wrong bus configuration!");
    Serial.println("higher PJON_MAX_PACKETS if necessary.");
  }
  if(code == PJON_CONTENT_TOO_LONG) {
    Serial.print("Content is too long, length: ");
    Serial.println(data);
  }
};
*/
void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  const char * arr = payload;
  if(payload[0]=='M') {
    if(arr[1]=='A')mSel=1;
    if(payload[1]=='B')mSel=0;
    if(payload[1]=='C')hc595(3);
    if(payload[1]=='D')hc595(4);
    if(payload[1]=='E')hc595(5);
    if(payload[1]=='F')hc595(6);
  }
  /*
  String str = "";
  str.concat(arr);
  String SS = str.substring(0,1);
  if(SS == "P"){
    str.remove(0,1);
    pulse = str.toInt();
  }
  else if(SS == "X"){
    str.remove(0,1);
    ptrn = str.substring(0,2).toInt();
    str.remove(0,2);
    if (str.length() == 6) {
      char charbuf[8];
      str.toCharArray(charbuf, 8);
      long int rgb = strtol(charbuf, 0, 16); //=>rgb=0x001234FE;
      byte red = (byte)(rgb >> 16);
      byte grn = (byte)(rgb >> 8);
      byte blu = (byte)(rgb);
      r = int(red);
      g = int(grn);
      b = int(blu);
    }
  }
  else if(SS == "M"){
   str.remove(0,1);
   hc595(str.toInt());  
  }
  else{
 
  }
  */
};
/*
void receiver_function_2(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  const char * arr = payload;
};
*/
void loop() {
  aR(0);
  aR(1);
  aR(2);
  bus.receive(5000);
  bus.update();
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
