

// includes
#include <FastLED.h>
#include <Arduino.h> // using c++ std lib now or some shit
#include <PJONMaster.h> // Pjon Master
#include <SoftwareSerial.h> // HC-05
#include <Queue.h>
//#include <Wire.h> // i2c
// Define HW LEDs here (not yet implemented)
#define MASTER_LED 22 // Basically is it on?
#define PJON_ISSUE_LED 24 // Is there an issue?
//#define I2CADDR 8

#define DEBUG   //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.
#ifdef DEBUG    //Macros are usually in all capital letters.
  #define DPRINT(...)    Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //DPRINTLN is a macro, debug print with new line
  #define DFLUSH(...)	Serial.flush(__VA_ARGS__)
#else
  #define DPRINT(...)     //now defines a blank line
  #define DPRINTLN(...)   //now defines a blank line
  #define DFLUSH(...)
#endif

uint32_t t_millis; // tick tock
uint8_t bus_id[4] = {0, 0, 1, 53}; // aNCS Unique Bus ID :)
int masterTerm = 0; // This is the ID of a TFT called masterTerm. 
int debugMode = 1 ; // Set to 1 for debug on

PJONMaster<SoftwareBitBang> bus(bus_id); // MASTER SO ID 254
SoftwareSerial hc05(10,11); // Bluetooth rx, tx
// Declare device Struct
struct device { // struct for a device
  int id; // device id num
  String namee; // device name
};
// declare an array of our devices and a tracking number.
// Strips
device strips[100];
int numStrip = 0;
// TFTs
device term[30];
int numTerm = 0;
// Matrix Boards
device matrix[20];
int numMatrix = 0;
// Routers
device router[20];
int numRouter = 0;
// only ever going to be one of each of these so no need for an array
// AS YET UNIMPLEMENTED
device wizNetIn;
device wizNetOut;

bool PjonLedOn = false; // as yet not working
bool msgSwitch = 0; // Set to true if message is needing to be sent
String msgToSend = ""; // Container for the message
int msgSendId = 0; // ID to send to
int *msgSendList;
int msgSendListSize = 0;
String mastBPM = "97.5";
String i2cMsg = "";

// Function to remove gaps from the device arrays 
// Designed to be called by removeDevice(id)
void compressStruct(String type){
  if(type.startsWith("Str")){ // check type, 
     for(int i =0; i<numStrip; i++){ // for all known devices of that type
    if(strips[i].id == NULL);{ // removeDevices(id) wipes the id here so we have a "gap"
    DPRINT("FOUND EMPTY STRIP RECORD WITH ID");
    DPRINTLN(i);
    strips[i].id = strips[i+1].id; // Copy the next record downs id
    strips[i].namee = strips[i+1].namee; // Copy its name
    strips[i+1].id = NULL; // Make the next record null
    DPRINT("got name ");
    DPRINTLN(strips[i].namee);
    } // repeat until NumStrips
  }
    numStrip -= 1; // we have 1 less device now.
  }
 if(type.startsWith("Mat")){
    for(int i =0; i<numMatrix; i++){
    if(matrix[i].id == NULL);{
    DPRINT("FOUND EMPTY MATRIX RECORD WITH ID");
    DPRINTLN(i);
    matrix[i] = matrix[i+1];
    matrix[i+1].id = NULL;
    DPRINT("got name ");
    DPRINTLN(matrix[i].namee);
    }
  }
    numMatrix -= 1;
  }
 if(type.startsWith("Ter")){
  for(int i =0; i<numTerm; i++){
    if(term[i].id == NULL);{
    DPRINT("FOUND EMPTY TERMINAL RECORD WITH ID");
    DPRINTLN(i);
    term[i] = term[i+1];
    term[i+1].id = NULL;
    DPRINT("got name ");
    DPRINTLN(term[i].namee);
    }
  }
    numTerm -= 1;
  }
   if(type.startsWith("Rtr")){
    for(int i =0; i<numRouter; i++){
    if(router[i].id == NULL);{
    DPRINT("FOUND EMPTY ROUTER RECORD WITH ID");
    DPRINTLN(i);
    router[i] = router[i+1];
    router[i+1].id = NULL;
    DPRINT("got name ");
    DPRINTLN(router[i].namee);
    }
  }
    numRouter -= 1;
  }
};
// Function to remove a device, Called from error_handler
void removeDevice(int id){
  
  // findDevice by id


  
  bool chkStrps = false; // have we checked strips array?
  bool chkMatr = false; // have we checked matrix array?
  bool chkRtrs = false; // what about router array?
  bool chkTerm = false; // and finally terminals? 
  bool found = false; // Did we find the device??
  DPRINT("Removing device with ID ");
  DPRINTLN(id);
  while(!found){ // Run until found is true
    if(chkStrps && chkMatr && chkRtrs && chkTerm){ // We have looked in every array and found nothing?
      DPRINTLN("Checked all arrays, Found nothing. Assuming device failed to register.");
      found = true; // device isnt in an array and we need not worry
    }
    DPRINTLN("Not Found!!!");
    if(!chkStrps){
      DPRINTLN("Checking Strips");
      for(int i = 0; i< numStrip; i++){
        if(strips[i].id == id){ // Compare our id against each record
          found = true; // Lovely job its a strip
          DPRINT("Device ");
          DPRINT(id);
          DPRINTLN(" is a Strip");
          strips[i].id = NULL; // Assign a NULL value... Bye!
          compressStruct("Str"); // Run the function to remove gaps in the array. sending the type.
        } // end if id=id ;
      } // we checked every strip...
      chkStrps = true; // best not do that repeatedly
    }
    if(!chkMatr){ // you get the idea
      DPRINTLN("Checking Matrix");
      for(int i = 0; i< numMatrix; i++){
        if(matrix[i].id == id){
          found = true;
          DPRINT("Device ");
          DPRINT(id);
          DPRINTLN(" is a Matrix");
          matrix[i].id = NULL;
          compressStruct("Mat");
        }
      }
      chkMatr = true;
    }
    if(!chkTerm){
      DPRINTLN("Checking Terms");
      for(int i = 0; i<numTerm;i++){
        if(term[i].id == id){
          found = true;
          DPRINT("Device ");
          DPRINT(id);
          DPRINTLN(" is a Terminal");
          term[i].id = NULL;
          compressStruct("Ter");
        }
      }
      chkTerm = true;
    }
    if(!chkRtrs){
      DPRINTLN("Checking Rtrs");
      for(int i = 0; i<numRouter;i++){
        if(router[i].id == id){
          found = true;
          DPRINT("Device ");
          DPRINT(id);
          DPRINTLN(" is a Router");
          router[i].id = NULL;
          compressStruct("Rtr");
        }
      }
      chkRtrs = true;
    }
  }
}
// FindDEvice BY ID
int findDeviceByID(int id){
  bool chkStrps = false; // have we checked strips array?
  bool chkMatr = false; // have we checked matrix array?
  bool chkRtrs = false; // what about router array?
  bool chkTerm = false; // and finally terminals? 
  bool found = false; // Did we find the device??
  DPRINT("Removing device with ID ");
  DPRINTLN(id);
  while(!found){ // Run until found is true
    if(chkStrps && chkMatr && chkRtrs && chkTerm){ // We have looked in every array and found nothing?
      DPRINTLN("Checked all arrays, Found nothing. Assuming device failed to register.");
      found = true; // device isnt in an array and we need not worry
      return 0;
      bus.send(uint8_t(id),"Rst,",5);
    }
    DPRINTLN("Not Found!!!");
    if(!chkStrps){
      DPRINTLN("Checking Strips");
      for(int i = 0; i< numStrip; i++){
        if(strips[i].id == id){ // Compare our id against each record
          found = true; // Lovely job its a strip
          DPRINT("Device ");
          DPRINT(id);
          DPRINTLN(" is a Strip");
          return 1; // Run the function to remove gaps in the array. sending the type.
        } 
      } // we checked every strip...
      chkStrps = true; // best not do that repeatedly
    }
    if(!chkMatr){ // you get the idea
      DPRINTLN("Checking Matrix");
      for(int i = 0; i< numMatrix; i++){
        if(matrix[i].id == id){
          found = true;
          DPRINT("Device ");
          DPRINT(id);
          DPRINTLN(" is a Matrix");
          return 1;
        }
      }
      chkMatr = true;
    }
    if(!chkTerm){
      DPRINTLN("Checking Terms");
      for(int i = 0; i<numTerm;i++){
        if(term[i].id == id){
          found = true;
          DPRINT("Device ");
          DPRINT(id);
          DPRINTLN(" is a Terminal");
          return 1;
        }
      }
      chkTerm = true;
    }
    if(!chkRtrs){
      DPRINTLN("Checking Rtrs");
      for(int i = 0; i<numRouter;i++){
        if(router[i].id == id){
          found = true;
          DPRINT("Device ");
          DPRINT(id);
          DPRINTLN(" is a Router");
          return 1;
        }
      }
      chkRtrs = true;
    }
  }
}
// PJON ERROR HANDLER CODE
void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  // Another Master / a static device dropped us?
  if (code == PJON_CONNECTION_LOST) { 
    DPRINT("PJON error: connection lost with device id ");
    bus.send(int(data),"Rst,",5);
    removeDevice(int(data));
  } 
  // we pinged a device we had already assigned an id to, and it didnt respond? 
  // we should remove it
  if (code == PJON_ID_ACQUISITION_FAIL) {
    DPRINT("PJONMaster error: connection lost with slave id ");
    DPRINTLN(data, DEC);
    int id = int(data);
    DPRINTLN(id);
    removeDevice(id); // pretty self explanitory
  }
  // NETWORK OVERLOAD. something is spamming us
  if (code == PJON_DEVICES_BUFFER_FULL) {
    DPRINT("PJONMaster error: master devices' buffer is full with a length of ");
    DPRINTLN(data);
  }
};
// function to return an ID given a type and a Name
int * findDeviceByName(String type, String nme){
  Queue<int> queue(numStrip+numMatrix+numTerm+numRouter);
  if(type.startsWith("Al")){
    for(int i=0;i<numStrip;i++){ // iterate all known devices in array
      DPRINT("Stepped into string");
      device dev = strips[i]; // get a device to compare
      DPRINT("Strip ");
      DPRINT(dev.id);
      DPRINT(dev.namee);
      DPRINTLN();
      if (dev.namee.compareTo(String(nme))== 0){ // an exact match gets you a 0
        DPRINT("found strip with name ");
        DPRINTLN(nme);
        queue.push(dev.id); // return the device id
      }
    }
    for(int i=0;i<numMatrix;i++){
      DPRINT("Stepped into Matrix");
      device dev = matrix[i];
      DPRINT("Matrix ");
      DPRINT(dev.id);
      DPRINT(dev.namee);
      DPRINTLN();
      if (dev.namee == String(nme)){
        DPRINT("found Matrix with name ");
        DPRINTLN(nme);
        queue.push(dev.id);
      }
    }
    for(int i=0;i<numTerm;i++){
      DPRINT("Stepped into Termi");
      device dev = term[i];
      DPRINT("Term ");
      DPRINT(dev.id);
      DPRINT(" - ");
      DPRINT(dev.namee);
      DPRINTLN();
      if (dev.namee == String(nme)){
        DPRINT("found Terminal with name ");
        DPRINTLN(nme);
        queue.push(dev.id);
        }
    }
    for(int i=0;i<numRouter;i++){
      DPRINT("Stepped into Router");
      device dev = router[i];
      DPRINT("Router ");
      DPRINT(dev.id);
      DPRINT(dev.namee);
      DPRINTLN();
      if (dev.namee == String(nme)){
        DPRINT("found Terminal with name ");
        DPRINTLN(nme);
        queue.push(dev.id);     }
    }
  }
  else if (type.startsWith("St")){ // look at the right array
    for(int i=0;i<numStrip;i++){ // iterate all known devices in array
      DPRINT("Stepped into string");
      device dev = strips[i]; // get a device to compare
      DPRINT("Strip ");
      DPRINT(dev.id);
      DPRINT(dev.namee);
      DPRINTLN();
      if (dev.namee.compareTo(String(nme))== 0){ // an exact match gets you a 0
        DPRINT("found strip with name ");
        DPRINTLN(nme);
        queue.push(dev.id); // return the device id
      }
    }
  }
  else if (type.startsWith("Ma")){
    for(int i=0;i<numMatrix;i++){
      DPRINT("Stepped into Matrix");
      device dev = matrix[i];
      DPRINT("Matrix ");
      DPRINT(dev.id);
      DPRINT(dev.namee);
      DPRINTLN();
      if (dev.namee == String(nme)){
        DPRINT("found Matrix with name ");
        DPRINTLN(nme);
        queue.push(dev.id);
      }
    }
  }
  else if (type.startsWith("Te")){
    for(int i=0;i<numTerm;i++){
      DPRINT("Stepped into Termi");
      device dev = term[i];
      DPRINT("Term ");
      DPRINT(dev.id);
      DPRINT(" - ");
      DPRINT(dev.namee);
      DPRINTLN();
      if (dev.namee == String(nme)){
        DPRINT("found Terminal with name ");
        DPRINTLN(nme);
        queue.push(dev.id);
        }
    }
  }
  else if (type.startsWith("Ro")){
    for(int i=0;i<numRouter;i++){
      DPRINT("Stepped into Router");
      device dev = router[i];
      DPRINT("Router ");
      DPRINT(dev.id);
      DPRINT(dev.namee);
      DPRINTLN();
      if (dev.namee == String(nme)){
        DPRINT("found Terminal with name ");
        DPRINTLN(nme);
        queue.push(dev.id);     }
      }
    }
    else {
      queue.push(0);
    }
    static int arr[255];
    DPRINT("got ");
    DPRINT(queue.count());
    DPRINT(" devices ");
    for(int i = 0; i<=queue.count();i++){
      arr[i]=queue.pop();
      DPRINT(arr[i]);
      DPRINT(" ");
    }
    DFLUSH();
    return arr;
 }

// Function to send to a group //
void sendMessageGroup(){
  DPRINTLN("GroupMess Function called");
  const char packet[msgToSend.length()+1]; // create a char array that is the length of the message +1 (for \0)
  msgToSend.toCharArray(packet,msgToSend.length()); //convert String msgToSend to char array, \0 auto added
  DPRINT("MESSAGE = "); 
  DPRINTLN(msgToSend);
  for(int i=0; i<=msgSendListSize; i++){
    DPRINT("SENDING TO");
    DPRINTLN(msgSendList[i]);
    DPRINT("Pjon status - ");
    bus.update();
    DPRINTLN(  bus.send(uint8_t(msgSendList[i]),packet,msgToSend.length()+1));
    bus.update();// adds the messsageto the bus (prints status code to console)
  }
  msgSendId = 0; //reset
  msgToSend = ""; //reset
  msgSwitch = 0; //reset
  msgSendListSize = 0;
  for(int i = 0; i<255; i++){
    msgSendList[i] = NULL;
  }
}
// function to send a message to a device, called from loop if msgSwitch = 1
void sendMessage(){
  msgSwitch = 0;
  if(msgSendId != 9999){
    DPRINTLN("sendMessage Function called");
    const char packet[msgToSend.length()+1]; // create a char array that is the length of the message +1 (for \0)
    msgToSend.toCharArray(packet,msgToSend.length()); //convert String msgToSend to char array, \0 auto added
    DPRINT("MESSAGE = "); 
    DPRINTLN(msgToSend);
    DPRINT("Pjon status - ");
    DPRINTLN(  bus.send(uint8_t(msgSendId),packet,msgToSend.length()+1));
    bus.update();// adds the messsageto the bus (prints status code to console)
  }
  else{
    DPRINTLN("GROUP MESSAGE");
    sendMessageGroup();
    
  }
  msgSendId = 0; //reset
  msgToSend = ""; //reset
  msgSwitch = 0; //reset
}
// function to register a device type and name
void regDev(int id, String reg){ // id is who it came from and reg is Type,Name,
  if( reg.startsWith("Str")){ // we are a strip, reg = Str,Name
    DPRINT(reg);
      reg.remove(0,reg.indexOf(',')+1); // removes the type and the trailing comma
      DPRINTLN(reg); // now its just Name
      strips[numStrip].id = id; // store the ID
      strips[numStrip].namee = reg; // store the name
      DPRINT("Strip : ");
      DPRINT(numStrip);
      DPRINT(" name : ");
      DPRINT(strips[numStrip].namee);
      DPRINT(" id : ");
      DPRINTLN(strips[numStrip].id);
      numStrip++; // We have 1 more strip
  }
  else if( reg.startsWith("Mat")){
    DPRINT(reg);
      reg.remove(0,reg.indexOf(',')+1);
      DPRINTLN(reg);
      matrix[numMatrix].id = id;
      matrix[numMatrix].namee = reg;
      DPRINT("Matrix : ");
      DPRINT(numMatrix);
      DPRINT(" name : ");
      DPRINT(matrix[numMatrix].namee);
      DPRINT(" id : ");
      DPRINTLN(matrix[numMatrix].id);
      numMatrix++;
  }
  else if( reg.startsWith("Ter")){
    DPRINT(reg);
      reg.remove(0,reg.indexOf(',')+1);
      DPRINTLN(reg);
      if( reg.startsWith("Master")){ // If a TFT registers, Called Master
        masterTerm = id; // make sure we send our debug output to it
      }
      term[numTerm].id = id;
      term[numTerm].namee = reg;
      DPRINT("Term : ");
      DPRINT(numTerm);
      DPRINT(" name : ");
      DPRINT(term[numTerm].namee);
      DPRINT(" id : ");
      DPRINTLN(term[numTerm].id);
      numTerm++;
  }
  else if( reg.startsWith("Rou")){
    DPRINT(reg);
      reg.remove(0,reg.indexOf(',')+1);
      DPRINTLN(reg);
      router[numRouter].id = id;
      router[numRouter].namee = reg;
      DPRINT("Router : ");
      DPRINT(numRouter);
      DPRINT(" name : ");
      DPRINT(router[numRouter].namee);
      DPRINT(" id : ");
      DPRINTLN(router[numRouter].id);
      numRouter++;
  }
  else if( reg.startsWith("WNI")){
    DPRINT(reg);
    reg.remove(0,reg.indexOf(',')+1);
      DPRINTLN(reg);
      wizNetIn.id = id;
      wizNetIn.namee = reg;
      DPRINT("OSC In : ");
      DPRINT(wizNetIn.namee);
      DPRINT(" id : ");
      DPRINTLN(wizNetIn.id);
  }
  DFLUSH();
  // send back a little "ack"
  msgSwitch = 1; 
  msgToSend = "ack,";
  msgSendId = id;
}
void parseMsg(int id, String msg) {
  //DPRINTLN("#msgparser");
  /* Message Struct as Follows 
   *  Reg,Str,Tree1 / POLL,NAME,TYPE
   *  Ctl,Str,Tree1,X1 / CONTROL,TYPE,NAME,COMMAND
   *  Ctl,56,S30 / CONTROL,ID#,COMMAND
   */ 
  //DPRINTLN(msg);
  int i = msg.indexOf(','); // A well structured message will have at least 1 comma
  if(i==-1)DPRINTLN("WHAT?"); // no comma? dont wanna parse your shit
  // Device Registration - msg = Reg,Type,Name
  if(msg.startsWith("Reg")){
    msg.remove(0,msg.indexOf(',')+1); // Take "Reg," off the front. 
    DPRINT(msg);
    regDev(id,msg); // pass it onto regDev function. 
  }
  // Manually Remove a device - msg = Rem,id#
  if(msg.startsWith("Rem")){
    msg.remove(0,msg.indexOf(',')+1); //takes the "Rem," off the front
    DPRINT(msg);
    removeDevice(id); // removes a device. 
  }
  // Check if you are registered
  if(msg.startsWith("Chk")){
    msg.remove(0,msg.indexOf(',')+1);
    findDeviceByID(id);
  }
  // Control a device - msg = Ctl,id#,MessgeToSend  OR  Ctl,Type,Name,MessgeToSend
 if (msg.startsWith("Ctl")){
    msg.remove(0,msg.indexOf(',')+1); // get rid of "Ctl,"
    DPRINTLN(msg);
    String idStr = msg.substring(0,msg.indexOf(',')); // take whatever comes before the next comma.
    DPRINTLN(idStr);
    int id = idStr.toInt(); // doing toInt() on something with any character other than a number in it results in 0 being returned
    msg.remove(0,msg.indexOf(',')+1); // remove the value (whatever it is) and the trailing comma
    int *idList;
    if ( id == 0 ) { // Theres a letter in there. so we got a type.
      String idName = msg.substring(0,msg.indexOf(',')); // Theres going to be name following. 
      msg.remove(0,msg.indexOf(',')+1); // strip off the name (and ",") leaving just the message to send
	    idList = findDeviceByName(idStr,idName);
	    msgSendId = 9999; // Run findDeviceByName with our 2 variables. Set the id to be the number returned
      msgSendList = idList;
      msgSendListSize = sizeof(idList);
      }
    msgSwitch = 1; // set ourselves up to send a message to someone
    msgToSend = msg; // copy the remaining message to our sending variable
    msg = ""; // delete the remaining message input
     // set the ID to send to -1 triggering group send
    // Messge will be sent in loop()
    DPRINTLN("MESSAGE TO SEND");
    DPRINT(msgToSend);
    DPRINT(" ID to send to ");
    DPRINT(id);
    DPRINTLN();
 } 
 // we got a reply from a device, just shunt it onto our terminal. 
 if(msg.startsWith("Rpl")){
  msg.remove(0,msg.indexOf(',')+1);
  msgSwitch = 1;
  msgToSend = msg;
  msg = "";
  msgSendId = 0;
 }
/*
 if(msg.startsWith("Clk")){
  for(int i=0; i< numStrip; i++){
    id = strips[i].id;
    bus.send(id,"Clk,1,",8);
    bus.update();
  }
} */
if(msg.startsWith("Kic")){
  for(int i = 0; i < numStrip; i++){
    id = strips[i].id;
    bus.send(id,"Kick,1,",8);
    bus.update();
  }
}
if(msg.startsWith("Bpm")){
  msg.remove(0,msg.indexOf(",")+1);
  String x = msg.substring(0,msg.indexOf(","));
  msg.remove(0,msg.indexOf(",")+1);
  if (x.compareTo(mastBPM)!=0){
    mastBPM = x;
    msgToSend = "Bpm,";
    msgToSend.concat(x);
    msgToSend.concat(",");
    msgSendId = 0;
    msgSwitch = 1;
  }
  
}
};
// PJON Reciver 
void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  //const char * arr = payload;
  int id = int(packet_info.sender_id);
  String toParse = payload; // conver to String
  parseMsg(id, toParse); // Shunt off to the PArser
  //DPRINTLN("MESSAGE!!");
} // Thats it!
/*
void gotWire(int howMany){
  while (Wire.available()){
    i2cMsg.concat(char(Wire.read()));
  }
  DPRINT("Got i2c - ");
  DPRINTLN(i2cMsg);
  parseMsg(99,i2cMsg);
  i2cMsg = "";
  
}
*/
// These print every known device to console. 
void printStrips(){
  DPRINT("Got a list of ");
  DPRINT(numStrip);
  DPRINTLN(" Strips");
  if (debugMode == true){
    for(int i = 0; i<numStrip; i++){
      DPRINT("Strip : ");
      DPRINT(i);
      DPRINT(" name : ");
      DPRINT(strips[i].namee);
      DPRINT(" id : ");
      DPRINTLN(strips[i].id);
    }
  }
}

void printMatrix(){
  DPRINT("Got a list of ");
  DPRINT(numMatrix);
  DPRINTLN(" Matrices");
  if (debugMode == true){
    for(int i = 0; i<numMatrix; i++){
      DPRINT("Matrix : ");
      DPRINT(i);
      DPRINT(" name : ");
      DPRINT(matrix[i].namee);
      DPRINT(" id : ");
      DPRINTLN(matrix[i].id);
    }
  }
}

void printTFTs(){
  DPRINT("Got a list of ");
  DPRINT(numTerm);
  DPRINTLN(" TFTs");
  if (debugMode == true){
    for(int i = 0; i<numTerm; i++){
      DPRINT("Term : ");
      DPRINT(i);
      DPRINT(" name : ");
      DPRINT(term[i].namee);
      DPRINT(" id : ");
      DPRINTLN(term[i].id);
    }
  }
}

void printRtrs(){
  DPRINT("Got a list of ");
  DPRINT(numRouter);
  DPRINTLN(" Routers");
  if (debugMode == true){
    for(int i = 0; i<numRouter; i++){
      DPRINT("Router : ");
      DPRINT(i);
      DPRINT(" name : ");
      DPRINT(router[i].namee);
      DPRINT(" id : ");
      DPRINTLN(router[i].id);
    }
  }
}

void setup() { // Setup
#ifdef DEBUG{
  Serial.begin(115200); // Start Serial
  #endif
  DPRINT("Setup ");
  hc05.begin(38400); // Start Bluetooth
  DPRINT(". ");
  bus.debug = true; // Pjon Debug switch
  DPRINT(". ");
  bus.strategy.set_pin(12); // Pjon Pin select
  DPRINT(". ");
  bus.set_receiver(receiver_function); // Function to run when PJON receives
  DPRINT(". ");
  bus.set_error(error_handler); // Function to run when PJON has an Error
  DPRINT(". ");
  bus.begin(); // Fire up the Bus
  DPRINT(". ");
  // RESET EVERY DEVICE ON THE BUS
  for (uint8_t i = 0; i < PJON_MAX_DEVICES; i++) {
      if (bus.ids[i].state) {
        bus.send(i,"Rst,",5);
      }
  }
  DPRINT(". ");
  bus.update(); // force a Pjon update ASSERT YOUR DOMINANCE HERE!
  DPRINT(". ");
  //digitalWrite(MASTER_LED,HIGH);
  t_millis = millis(); // tick tock bitches.
  DPRINT(". ");
  //Wire.begin(8);
  DPRINT(". ");
  //Wire.onReceive(gotWire);
  }
  //MAin fucking loop time.
  
void loop() {
  if(bus.debug)digitalWrite(PJON_ISSUE_LED,HIGH); // Turn the led on if debug is on. 
  if (millis() - t_millis > 10000) { // after 10s
    // Check if registered slaves are still present on the bus
    bus.check_slaves_presence();
    // List all slaves here
    DPRINTLN("List of slaves known by master: ");
    for (uint8_t i = 0; i < PJON_MAX_DEVICES; i++) {
      if (bus.ids[i].state) {
        DPRINT(" - Device id: ");
        DPRINT(i + 1); // Shifted by one to avoid PJON_BROADCAST
        DPRINT(" Device rid: ");
        DPRINT(bus.ids[i].rid);
      } 
    }
    DPRINTLN();
    DFLUSH(); // Make sure the serial finishes printing. 
    t_millis = millis(); // tick tock
  }
  if(msgSwitch!=0){ // Theres a message to send fam!
  sendMessage(); // Send it then!
  }
  bus.receive(5000); // PJON Listen 
  bus.update(); // 
  if (hc05.available()) {  // Got bluetooth ?? 
    String str = "";
    DPRINT("got BT");
    // read all those lovely characters into a string Thanks!
    while ( hc05.available()) {
      char c = hc05.read();
      str.concat(c); // < - lovely string.... 
    }
    if (str != "") { // Message wasnt empty, Nice!
      DPRINTLN(str);
      DPRINTLN("sending to parser");
      parseMsg(int(999), str); // send tto parser with id 99 (could cause issues later tbf)
      str = ""; // empty that shit!
    }
  }
  EVERY_N_SECONDS(10){ // This is why we include FastLEDS... will replace with some millis bullshit
    if(bus.debug){ // do the debug shiz
      if(!PjonLedOn){ // Turn that shit on
        digitalWrite(PJON_ISSUE_LED,HIGH); // boom 
        PjonLedOn = true; // better keep track of that
      }
      else{
        digitalWrite(PJON_ISSUE_LED,LOW); // no led
        PjonLedOn = false; // better keep that
      }
    }
    printStrips(); // list all my devices thanks
    printMatrix(); // all of them
    printTFTs();
    printRtrs();
  }
  // allows direct serial input
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
}; 
// thats all folks
