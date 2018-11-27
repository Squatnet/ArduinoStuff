// 022_R Router With PJON on 12 and 7, 3 analog pots and 1x 74hc595 LED array.
// Aim : to Receive pulse signals and relay messages to "branch" of lights and relay board

//breakdown code for debug
#define fL(x,a,b) for(int x=a;x<b;x++)
#include <PJON.h>
uint8_t bus_id[4] = {0,0,1,53};
uint8_t bus_id2[4] = {0,0,0,1};
// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> bus(bus_id, 30); // 0,0,1,53::30
//PJON<SoftwareBitBang> bus2(bus_id2, 1); // 0,0,0,1::1
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
int mSel ;
int ptrn = 0;
int pulse = 0;
int r = 0;
int g = 0;
int b = 0;
bool flag1 = 0 ;
String debug = "";
bool flag2 = 0;
String ledMsg = "";
int ledNum = 0;

void aR(int bb){
  val[bb] = map(analogRead(bb), 0, 1023, 0, 255); // if menu.x==# map to x values
  if(valM[bb] > (val[bb] + 3) | valM[bb] < (val[bb] - 3 )) { //if memory if greater than or less than value +- a little bit to allow a buffer for loose readings
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
    //Serial.println("Analog Loop OK");
  }
}
void hc595(int aa) {
  data = dataArray[aa];
        digitalWrite(LPIN, 0);
        shiftOut(DPIN, CPIN, data);
        digitalWrite(LPIN, 1);
}

void setup() {
  //Serial.begin(115200);
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
//  bus.strategy.set_pins(7); // sub bus
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
  //Serial.println("Setup Complete");
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
  String str = "";
  str.concat(arr);
  //Serial.println("Arr = " + str);
  String SS = str.substring(0,1);
  str.remove(0,1);
  if(SS=="M") {
    //Serial.println( mSel);
    String SSS = str.substring(0,1);
    //Serial.println("HEre" + SSS);
    
    str.remove(0,1);
    //Serial.println( "here again" + SSS);
    if(SSS=="A") mSel = 0;
    if(SSS=="B") mSel = 1;
    if(SSS=="C") mSel = 2;
    if(SSS=="D") mSel = 3;
    if(SSS=="E") mSel = 4;
    if(SSS=="F") mSel = 5;
  }
  else if(SS == "R"){
   // Serial.println("Call HC595");
    String SSS = str.substring(0,1);
    str.remove(0,1);
    if(SSS=="A")hc595(0);
    if(SSS=="B")hc595(1);
    if(SSS=="C")hc595(2);
    if(SSS=="D")hc595(3);
    if(SSS=="E")hc595(4);
    if(SSS=="F")hc595(5);
  }
  else if(SS == "L"){
    debug.concat(arr);
    flag1=1;
    String SSS = str.substring(0,1);
    //Serial.println("Call LEDS");
   // Serial.println("String = " + str);
    str.remove(0,1);
    if(SSS=="A")ledNum = 1;
    if(SSS=="B")ledNum = 2;
    if(SSS=="C")ledNum = 3;
    if(SSS=="D")ledNum = 4;
    if(SSS=="E")ledNum = 5;
    if(SSS=="F")ledNum = 6;
    ledMsg = str;
    flag2 = 1;
  }
  else {
    debug.concat(arr);
    flag1=1;
    
  }
};

void debugSender(){
  flag1 = 0 ;
  char buffe[debug.length()+1];
  debug.toCharArray(buffe,debug.length()+1);
  bus.send_packet(10, bus_id, buffe, debug.length()+1);
  debug = "";
}
/*void receiver_function_2(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  const char * arr = payload;
  String str2 = "";
  str2.concat(arr);
  bus.send_packet(10,arr,length);
    //Serial.println("Just Received : " + str2);
};
*/
void loop() {
  aR(0);
  aR(1);
  aR(2);
  if(flag1 == 1) debugSender();
  if (flag2 == 1) sendLED();
  //hc595(6);
  bus.receive(5000);
  //bus2.receive(100);
  bus.update();
  //hc595(0);
  //bus2.update();
  //Serial.println("Gimme Datt LOOP");
  }
void sendLED(){
  ledNum++; 
  flag2 = 0;
  char buff[ledMsg.length()+1];
  ledMsg.toCharArray(buff,ledMsg.length()+1);
  //int i =0;
  //while(buff[i] != '\0')i++;
 bus.send_packet(10,bus_id,buff,ledMsg.length());
 bus.send_packet(ledNum, bus_id2, buff, ledMsg.length());
 //Serial.println(data + "sendLED");
 ledMsg = "";
 
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
