#include <FastLED.h>
#include <Arduino.h>
#include <PJONMaster.h>
#include <SoftwareSerial.h>
#define MASTER_LED 22
#define PJON_ISSUE_LED 24

uint8_t bus_id[4] = {0, 0, 0, 1}; // aNCS Unique Bus ID :)
uint32_t t_millis;
PJONMaster<SoftwareBitBang> bus(bus_id); // MASTER SO ID 254
SoftwareSerial hc05(23,22); // rx, tx
/* ALLOWED IDS
    1 = Master
    2 - 100 = Trees
    - 1 - 4 = Branches (seperate bus)
    101 - 140 = Matrix
    150 - 180 = ?
    200 - 230 = Serial
*/
/*
   SPARE IDS : 141 - 149, 181 - 199, 231 - 253
*/

// Declare Structs
struct device { // struct for a device
  int id; // device id num
  String namee; // device name
};
int debugMode = 0 ;

device strips[100];
int numStrip = 0;
device term[30];
int numTerm = 0;
device matrix[20];
int numMatrix = 0;
device router[20];
int numRouter = 0;
device wizNetIn;
device wizNetOut;

bool PjonLedOn = false;
bool msgSwitch = 0;
String msgToSend = "";
int msgSendId = 0;
/* void compressStr(){
  for(int i=0;i<=numStrip;i++){
    if (strips[i].id = NULL){
            
    }
  }
}; */
void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if (code == PJON_CONNECTION_LOST) {
    Serial.print("PJON error: connection lost with device id ");
    Serial.println((uint8_t)bus.packets[data].content[0], DEC);
  }
  if (code == PJON_ID_ACQUISITION_FAIL) {
    Serial.print("PJONMaster error: connection lost with slave id ");
    Serial.println(data, DEC);
  }
  if (code == PJON_DEVICES_BUFFER_FULL) {
    Serial.print("PJONMaster error: master devices' buffer is full with a length of ");
    Serial.println(data);
  }
};
int findDeviceByName(String type, String nme){
  if (type.startsWith("St")){
    for(int i=0;i<numStrip;i++){
      Serial.print("Stepped into string");
      device dev = strips[i];
      Serial.print("Strip ");
      Serial.print(dev.id);
      Serial.print(dev.namee);
      Serial.println();
      if (dev.namee.compareTo(String(nme))== 0){
        Serial.print("found strip with name ");
        Serial.println(nme);
        return dev.id;
      }
    }
  }
  else if (type.startsWith("Ma")){
    for(int i=0;i<numMatrix;i++){
      Serial.print("Stepped into Matrix");
      device dev = matrix[i];
      Serial.print("Matrix ");
      Serial.print(dev.id);
      Serial.print(dev.namee);
      Serial.println();
      if (dev.namee == String(nme)){
        Serial.print("found Matrix with name ");
        Serial.println(nme);
        return dev.id;
      }
    }
  }
  else if (type.startsWith("Te")){
    for(int i=0;i<numTerm;i++){
      Serial.print("Stepped into Termi");
      device dev = term[i];
      Serial.print("Term ");
      Serial.print(dev.id);
      Serial.print(dev.namee);
      Serial.println();
      if (dev.namee == String(nme)){
        Serial.print("found Terminal with name ");
        Serial.println(nme);
        return dev.id;
      }
    }
  }
  else if (type.startsWith("Ro")){
    for(int i=0;i<numTerm;i++){
      Serial.print("Stepped into Router");
      device dev = router[i];
      Serial.print("Router ");
      Serial.print(dev.id);
      Serial.print(dev.namee);
      Serial.println();
      if (dev.namee == String(nme)){
        Serial.print("found Terminal with name ");
        Serial.println(nme);
        return dev.id;
      }
    }
  }
  else return 99999; 
}
void sendMessage(){
  const char packet[msgToSend.length()+1];
  msgToSend.toCharArray(packet,msgToSend.length());
  bus.send_packet(msgSendId,packet,msgToSend.length()+1);
  msgSendId = 0;
  msgToSend = "";
  msgSwitch = 0;
}
void regDev(int id, String reg){
  if( reg.startsWith("Str")){
    Serial.print(reg);
      reg.remove(0,4);
      Serial.println(reg);
      strips[numStrip].id = id;
      strips[numStrip].namee = reg;
      Serial.print("Strip : ");
      Serial.print(numStrip);
      Serial.print(" name : ");
      Serial.print(strips[numStrip].namee);
      Serial.print(" id : ");
      Serial.println(strips[numStrip].id);
      numStrip++;
  }
  else if( reg.startsWith("Mat")){
    Serial.print(reg);
      reg.remove(0,4);
      Serial.println(reg);
      matrix[numMatrix].id = id;
      matrix[numMatrix].namee = reg;
      Serial.print("Matrix : ");
      Serial.print(numMatrix);
      Serial.print(" name : ");
      Serial.print(matrix[numMatrix].namee);
      Serial.print(" id : ");
      Serial.println(matrix[numMatrix].id);
      numMatrix++;
  }
  else if( reg.startsWith("Ter")){
    Serial.print(reg);
      reg.remove(0,4);
      Serial.println(reg);
      term[numTerm].id = id;
      term[numTerm].namee = reg;
      Serial.print("Term : ");
      Serial.print(numTerm);
      Serial.print(" name : ");
      Serial.print(term[numTerm].namee);
      Serial.print(" id : ");
      Serial.println(term[numTerm].id);
      numTerm++;
  }
  else if( reg.startsWith("Rou")){
    Serial.print(reg);
      reg.remove(0,4);
      Serial.println(reg);
      router[numRouter].id = id;
      router[numRouter].namee = reg;
      Serial.print("Router : ");
      Serial.print(numRouter);
      Serial.print(" name : ");
      Serial.print(router[numRouter].namee);
      Serial.print(" id : ");
      Serial.println(router[numRouter].id);
      numRouter++;
  }
  else if( reg.startsWith("WNI")){
    Serial.print(reg);
    reg.remove(0,4);
      Serial.println(reg);
      wizNetIn.id = id;
      wizNetIn.namee = reg;
      Serial.print("OSC In : ");
      Serial.print(wizNetIn.namee);
      Serial.print(" id : ");
      Serial.println(wizNetIn.id);
  }
}
void parseMsg(int id, String msg) {
  Serial.println("#msgparser");
  /* Message Struct as Follows 
   *  Reg,Str,Tree1 / POLL,NAME,TYPE
   *  Ctl,Str,Tree1,X1 / CONTROL,TYPE,NAME,COMMAND
   *  Ctl,56,S30 / CONTROL,ID#,COMMAND
   */ 
  Serial.println(msg);
  int i = msg.indexOf(',');
  if(i==-1)Serial.println("WHAT?");
  if(msg.startsWith("Reg")){
    msg.remove(0,4);
    Serial.print(msg);
    regDev(id,msg);
  }
 if (msg.startsWith("Ctl")){
    msg.remove(0,4);
    Serial.println(msg);
    String idStr = msg.substring(0,msg.indexOf(','));
    int id = idStr.toInt();
    msg.remove(0,msg.indexOf(',')+1);
    if ( id == 0 ) {
      String idName = msg.substring(0,msg.indexOf(','));
      msg.remove(0,msg.indexOf(',')+1); 
      id = findDeviceByName(idStr,idName);   
      Serial.println("ID");
      Serial.println(id);
      }
    msgSwitch = 1;
    msgToSend = msg;
    msg = "";
    msgSendId = id;
    Serial.println("MESSAGE TO SEND");
    Serial.print(msgToSend);
    Serial.print(id);
    Serial.println();
 }
}
void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  //const char * arr = payload;
  int id = int(packet_info.sender_id);
  String toParse = payload;
  parseMsg(id, toParse);
  Serial.println("MESSAGE!!");
}

void printStrips(){
  Serial.print("Got a list of ");
  Serial.print(numStrip);
  Serial.println(" Strips");
  if (debugMode == true){
    for(int i = 0; i<numStrip; i++){
      Serial.print("Strip : ");
      Serial.print(i);
      Serial.print(" name : ");
      Serial.print(strips[i].namee);
      Serial.print(" id : ");
      Serial.println(strips[i].id);
    }
  }
}

void printMatrix(){
  Serial.print("Got a list of ");
  Serial.print(numMatrix);
  Serial.println(" Matrices");
  if (debugMode == true){
    for(int i = 0; i<numMatrix; i++){
      Serial.print("Matrix : ");
      Serial.print(i);
      Serial.print(" name : ");
      Serial.print(matrix[i].namee);
      Serial.print(" id : ");
      Serial.println(matrix[i].id);
    }
  }
}

void printTFTs(){
  Serial.print("Got a list of ");
  Serial.print(numTerm);
  Serial.println(" TFTs");
  if (debugMode == true){
    for(int i = 0; i<numTerm; i++){
      Serial.print("Term : ");
      Serial.print(i);
      Serial.print(" name : ");
      Serial.print(term[i].namee);
      Serial.print(" id : ");
      Serial.println(term[i].id);
    }
  }
}

void printRtrs(){
  Serial.print("Got a list of ");
  Serial.print(numRouter);
  Serial.println(" Routers");
  if (debugMode == true){
    for(int i = 0; i<numRouter; i++){
      Serial.print("Router : ");
      Serial.print(i);
      Serial.print(" name : ");
      Serial.print(router[i].namee);
      Serial.print(" id : ");
      Serial.println(router[i].id);
    }
  }
}
void setup() {
  Serial.begin(115200);
  hc05.begin(38400);
  Serial.print("Setup");
  bus.debug = true;
  bus.strategy.set_pin(12);
  bus.set_receiver(receiver_function);
  bus.set_error(error_handler);
  bus.begin();
  //digitalWrite(MASTER_LED,HIGH);
  //if (bus.debug)bus.send_repeatedly(PJON_BROADCAST, "Master says hi!", 15, 2500000);
  t_millis = millis();
}
void loop() {
  if(bus.debug)digitalWrite(PJON_ISSUE_LED,HIGH);
  if (millis() - t_millis > 10000) {
    // Check if registered slaves are still present on the bus
    bus.check_slaves_presence();

    Serial.println("List of slaves known by master: ");
    for (uint8_t i = 0; i < PJON_MAX_DEVICES; i++) {
      if (bus.ids[i].state) {
        Serial.print(" - Device id: ");
        Serial.print(i + 1); // Shifted by one to avoid PJON_BROADCAST
        Serial.print(" Device rid: ");
        Serial.print(bus.ids[i].rid);
      }
    }
    Serial.println();
    Serial.flush();
    t_millis = millis();
  }
  if(msgSwitch!=0){
  sendMessage();
  }
  bus.receive(5000);
  bus.update();
  if (hc05.available()) {
    String str = "";
    Serial.print("got BT");
    while ( hc05.available()) {
      char c = hc05.read();
      str.concat(c);
    }
    if (str != "") {
      Serial.println(str);
      Serial.println("sending to parser");
      parseMsg(int(99), str);
      str = "";
    }
  }
  EVERY_N_SECONDS(5){ 
    if(bus.debug){
      if(!PjonLedOn){
        digitalWrite(PJON_ISSUE_LED,HIGH);
        PjonLedOn = true;
      }
      else{
        digitalWrite(PJON_ISSUE_LED,LOW);
        PjonLedOn = false;
      }
    }
    printStrips();
    printMatrix();
    printTFTs();
    printRtrs();
  }
  if (Serial.available()){
    String str = "";
    while ( Serial.available()) {
      char c = Serial.read();
      str.concat(c);
      delay(30);
    }
    if (str != "") {
      parseMsg(int(99), str);
      str = "";
    }
  }
}
