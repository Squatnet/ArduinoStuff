// includes
#include <FastLED.h>
#include <Arduino.h> // using c++ std lib now or some shit
#include <PJONMaster.h> // Pjon Master
#include <SoftwareSerial.h> // HC-05
// Define HW LEDs here (not yet implemented)
#define MASTER_LED 22 // Basically is it on?
#define PJON_ISSUE_LED 24 // Is there an issue?


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

// Function to remove gaps from the device arrays 
// Designed to be called by removeDevice(id)
void compressStruct(String type){
  if(type.startsWith("Str")){ // check type, 
     for(int i =0; i<numStrip; i++){ // for all known devices of that type
    if(strips[i].id == NULL);{ // removeDevices(id) wipes the id here so we have a "gap"
    Serial.print("FOUND EMPTY STRIP RECORD WITH ID");
    Serial.println(i);
    strips[i].id = strips[i+1].id; // Copy the next record downs id
    strips[i].namee = strips[i+1].namee; // Copy its name
    strips[i+1].id = NULL; // Make the next record null
    Serial.print("got name ");
    Serial.println(strips[i].namee);
    } // repeat until NumStrips
  }
    numStrip -= 1; // we have 1 less device now.
  }
 if(type.startsWith("Mat")){
    for(int i =0; i<numMatrix; i++){
    if(matrix[i].id == NULL);{
    Serial.print("FOUND EMPTY MATRIX RECORD WITH ID");
    Serial.println(i);
    matrix[i] = matrix[i+1];
    matrix[i+1].id = NULL;
    Serial.print("got name ");
    Serial.println(matrix[i].namee);
    }
  }
    numMatrix -= 1;
  }
 if(type.startsWith("Ter")){
  for(int i =0; i<numTerm; i++){
    if(term[i].id == NULL);{
    Serial.print("FOUND EMPTY TERMINAL RECORD WITH ID");
    Serial.println(i);
    term[i] = term[i+1];
    term[i+1].id = NULL;
    Serial.print("got name ");
    Serial.println(term[i].namee);
    }
  }
    numTerm -= 1;
  }
   if(type.startsWith("Rtr")){
    for(int i =0; i<numRouter; i++){
    if(router[i].id == NULL);{
    Serial.print("FOUND EMPTY ROUTER RECORD WITH ID");
    Serial.println(i);
    router[i] = router[i+1];
    router[i+1].id = NULL;
    Serial.print("got name ");
    Serial.println(router[i].namee);
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
  Serial.print("Removing device with ID ");
  Serial.println(id);
  while(!found){ // Run until found is true
    if(chkStrps && chkMatr && chkRtrs && chkTerm){ // We have looked in every array and found nothing?
      Serial.println("Checked all arrays, Found nothing. Assuming device failed to register.");
      found = true; // device isnt in an array and we need not worry
    }
    Serial.println("Not Found!!!");
    if(!chkStrps){
      Serial.println("Checking Strips");
      for(int i = 0; i< numStrip; i++){
        if(strips[i].id == id){ // Compare our id against each record
          found = true; // Lovely job its a strip
          Serial.print("Device ");
          Serial.print(id);
          Serial.println(" is a Strip");
          strips[i].id = NULL; // Assign a NULL value... Bye!
          compressStruct("Str"); // Run the function to remove gaps in the array. sending the type.
        } // end if id=id ;
      } // we checked every strip...
      chkStrps = true; // best not do that repeatedly
    }
    if(!chkMatr){ // you get the idea
      Serial.println("Checking Matrix");
      for(int i = 0; i< numMatrix; i++){
        if(matrix[i].id == id){
          found = true;
          Serial.print("Device ");
          Serial.print(id);
          Serial.println(" is a Matrix");
          matrix[i].id = NULL;
          compressStruct("Mat");
        }
      }
      chkMatr = true;
    }
    if(!chkTerm){
      Serial.println("Checking Terms");
      for(int i = 0; i<numTerm;i++){
        if(term[i].id == id){
          found = true;
          Serial.print("Device ");
          Serial.print(id);
          Serial.println(" is a Terminal");
          term[i].id = NULL;
          compressStruct("Ter");
        }
      }
      chkTerm = true;
    }
    if(!chkRtrs){
      Serial.println("Checking Rtrs");
      for(int i = 0; i<numRouter;i++){
        if(router[i].id == id){
          found = true;
          Serial.print("Device ");
          Serial.print(id);
          Serial.println(" is a Router");
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
  Serial.print("Removing device with ID ");
  Serial.println(id);
  while(!found){ // Run until found is true
    if(chkStrps && chkMatr && chkRtrs && chkTerm){ // We have looked in every array and found nothing?
      Serial.println("Checked all arrays, Found nothing. Assuming device failed to register.");
      found = true; // device isnt in an array and we need not worry
      return 0;
      bus.send(uint8_t(id),"Rst,",5);
    }
    Serial.println("Not Found!!!");
    if(!chkStrps){
      Serial.println("Checking Strips");
      for(int i = 0; i< numStrip; i++){
        if(strips[i].id == id){ // Compare our id against each record
          found = true; // Lovely job its a strip
          Serial.print("Device ");
          Serial.print(id);
          Serial.println(" is a Strip");
          return 1; // Run the function to remove gaps in the array. sending the type.
        } 
      } // we checked every strip...
      chkStrps = true; // best not do that repeatedly
    }
    if(!chkMatr){ // you get the idea
      Serial.println("Checking Matrix");
      for(int i = 0; i< numMatrix; i++){
        if(matrix[i].id == id){
          found = true;
          Serial.print("Device ");
          Serial.print(id);
          Serial.println(" is a Matrix");
          return 1;
        }
      }
      chkMatr = true;
    }
    if(!chkTerm){
      Serial.println("Checking Terms");
      for(int i = 0; i<numTerm;i++){
        if(term[i].id == id){
          found = true;
          Serial.print("Device ");
          Serial.print(id);
          Serial.println(" is a Terminal");
          return 1;
        }
      }
      chkTerm = true;
    }
    if(!chkRtrs){
      Serial.println("Checking Rtrs");
      for(int i = 0; i<numRouter;i++){
        if(router[i].id == id){
          found = true;
          Serial.print("Device ");
          Serial.print(id);
          Serial.println(" is a Router");
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
    Serial.print("PJON error: connection lost with device id ");
    Serial.println((uint8_t)bus.packets[data].content[0], DEC);
  } 
  // we pinged a device we had already assigned an id to, and it didnt respond? 
  // we should remove it
  if (code == PJON_ID_ACQUISITION_FAIL) {
    Serial.print("PJONMaster error: connection lost with slave id ");
    Serial.println(data, DEC);
    int id = int(data);
    Serial.println(id);
    removeDevice(id); // pretty self explanitory
  }
  // NETWORK OVERLOAD. something is spamming us
  if (code == PJON_DEVICES_BUFFER_FULL) {
    Serial.print("PJONMaster error: master devices' buffer is full with a length of ");
    Serial.println(data);
  }
};
// function to return an ID given a type and a Name
int findDeviceByName(String type, String nme){
  if (type.startsWith("St")){ // look at the right array
    for(int i=0;i<numStrip;i++){ // iterate all known devices in array
      Serial.print("Stepped into string");
      device dev = strips[i]; // get a device to compare
      Serial.print("Strip ");
      Serial.print(dev.id);
      Serial.print(dev.namee);
      Serial.println();
      if (dev.namee.compareTo(String(nme))== 0){ // an exact match gets you a 0
        Serial.print("found strip with name ");
        Serial.println(nme);
        return dev.id; // return the device id
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
      Serial.print(" - ");
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
  else return 99999; // got no device with that name,
}
// function to send a message to a device, called from loop if msgSwitch = 1
void sendMessage(){
  msgSwitch = 0;
  Serial.println("sendMessage Function called");
  const char packet[msgToSend.length()+1]; // create a char array that is the length of the message +1 (for \0)
  msgToSend.toCharArray(packet,msgToSend.length()); //convert String msgToSend to char array, \0 auto added
  Serial.print("MESSAGE = "); 
  Serial.println(msgToSend);
  Serial.print("Pjon status - ");
  Serial.println(  bus.send(uint8_t(msgSendId),packet,msgToSend.length()+1)); // adds the messsageto the bus (prints status code to console)
  msgSendId = 0; //reset
  msgToSend = ""; //reset
  msgSwitch = 0; //reset
}
// function to register a device type and name
void regDev(int id, String reg){ // id is who it came from and reg is Type,Name,
  if( reg.startsWith("Str")){ // we are a strip, reg = Str,Name
    Serial.print(reg);
      reg.remove(0,reg.indexOf(',')+1); // removes the type and the trailing comma
      Serial.println(reg); // now its just Name
      strips[numStrip].id = id; // store the ID
      strips[numStrip].namee = reg; // store the name
      Serial.print("Strip : ");
      Serial.print(numStrip);
      Serial.print(" name : ");
      Serial.print(strips[numStrip].namee);
      Serial.print(" id : ");
      Serial.println(strips[numStrip].id);
      numStrip++; // We have 1 more strip
  }
  else if( reg.startsWith("Mat")){
    Serial.print(reg);
      reg.remove(0,reg.indexOf(',')+1);
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
      reg.remove(0,reg.indexOf(',')+1);
      Serial.println(reg);
      if( reg.startsWith("Master")){ // If a TFT registers, Called Master
        masterTerm = id; // make sure we send our debug output to it
      }
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
      reg.remove(0,reg.indexOf(',')+1);
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
    reg.remove(0,reg.indexOf(',')+1);
      Serial.println(reg);
      wizNetIn.id = id;
      wizNetIn.namee = reg;
      Serial.print("OSC In : ");
      Serial.print(wizNetIn.namee);
      Serial.print(" id : ");
      Serial.println(wizNetIn.id);
  }
  // send back a little "ack"
  msgSwitch = 1; 
  msgToSend = "ack,";
  msgSendId = id;
}
void parseMsg(int id, String msg) {
  Serial.println("#msgparser");
  /* Message Struct as Follows 
   *  Reg,Str,Tree1 / POLL,NAME,TYPE
   *  Ctl,Str,Tree1,X1 / CONTROL,TYPE,NAME,COMMAND
   *  Ctl,56,S30 / CONTROL,ID#,COMMAND
   */ 
  Serial.println(msg);
  int i = msg.indexOf(','); // A well structured message will have at least 1 comma
  if(i==-1)Serial.println("WHAT?"); // no comma? dont wanna parse your shit
  // Device Registration - msg = Reg,Type,Name
  if(msg.startsWith("Reg")){
    msg.remove(0,msg.indexOf(',')+1); // Take "Reg," off the front. 
    Serial.print(msg);
    regDev(id,msg); // pass it onto regDev function. 
  }
  // Manually Remove a device - msg = Rem,id#
  if(msg.startsWith("Rem")){
    msg.remove(0,msg.indexOf(',')+1); //takes the "Rem," off the front
    Serial.print(msg);
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
    Serial.println(msg);
    String idStr = msg.substring(0,msg.indexOf(',')); // take whatever comes before the next comma.
    int id = idStr.toInt(); // doing toInt() on something with any character other than a number in it results in 0 being returned
    msg.remove(0,msg.indexOf(',')+1); // remove the value (whatever it is) and the trailing comma
    if ( id == 0 ) { // Theres a letter in there. so we got a type.
      String idName = msg.substring(0,msg.indexOf(',')); // Theres going to be name following. 
      msg.remove(0,msg.indexOf(',')+1); // strip off the name (and ",") leaving just the message to send
      id = findDeviceByName(idStr,idName);  // Run findDeviceByName with our 2 variables. Set the id to be the number returned
      Serial.println("ID");
      Serial.println(id);
      }
    msgSwitch = 1; // set ourselves up to send a message to someone
    msgToSend = msg; // copy the remaining message to our sending variable
    msg = ""; // delete the remaining message input
    msgSendId = id; // set the ID we with to send to
    // Messge will be sent in loop()
    Serial.println("MESSAGE TO SEND");
    Serial.print(msgToSend);
    Serial.print(" ID to send to ");
    Serial.print(id);
    Serial.println();
 } 
 // we got a reply from a device, just shunt it onto our terminal. 
 if(msg.startsWith("Rpl")){
  msg.remove(0,msg.indexOf(',')+1);
  msgSwitch = 1;
  msgToSend = msg;
  msg = "";
  msgSendId = masterTerm;
 }
}
// PJON Reciver 
void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  //const char * arr = payload;
  int id = int(packet_info.sender_id);
  String toParse = payload; // conver to String
  parseMsg(id, toParse); // Shunt off to the PArser
  Serial.println("MESSAGE!!");
} // Thats it!

// These print every known device to console. 
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

void setup() { // Setup
  Serial.begin(115200); // Start Serial
  hc05.begin(38400); // Start Bluetooth
  Serial.print("Setup");
  bus.debug = true; // Pjon Debug switch
  bus.strategy.set_pin(12); // Pjon Pin select
  bus.set_receiver(receiver_function); // Function to run when PJON receives
  bus.set_error(error_handler); // Function to run when PJON has an Error
  bus.begin(); // Fire up the Bus
  
  // RESET EVERY DEVICE ON THE BUS
  for (uint8_t i = 0; i < PJON_MAX_DEVICES; i++) {
      if (bus.ids[i].state) {
        bus.send(i,"Rst,",5);
      }
  }
  
  bus.update(); // force a Pjon update ASSERT YOUR DOMINANCE HERE!
  //digitalWrite(MASTER_LED,HIGH);
  //if (bus.debug)bus.send_repeatedly(PJON_BROADCAST, "Master says hi!", 15, 2500000);
  t_millis = millis(); // tick tock bitches.
  }
  //MAin fucking loop time.
  
void loop() {
  if(bus.debug)digitalWrite(PJON_ISSUE_LED,HIGH); // Turn the led on if debug is on. 
  if (millis() - t_millis > 10000) { // after 10s
    // Check if registered slaves are still present on the bus
    bus.check_slaves_presence();
    // List all slaves here
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
    Serial.flush(); // Make sure the serial finishes printing. 
    t_millis = millis(); // tick tock
  }
  if(msgSwitch!=0){ // Theres a message to send fam!
  sendMessage(); // Send it then!
  }
  bus.receive(5000); // PJON Listen 
  bus.update(); // 
  if (hc05.available()) {  // Got bluetooth ?? 
    String str = "";
    Serial.print("got BT");
    // read all those lovely characters into a string Thanks!
    while ( hc05.available()) {
      char c = hc05.read();
      str.concat(c); // < - lovely string.... 
    }
    if (str != "") { // Message wasnt empty, Nice!
      Serial.println(str);
      Serial.println("sending to parser");
      parseMsg(int(99), str); // send tto parser with id 99 (could cause issues later tbf)
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
