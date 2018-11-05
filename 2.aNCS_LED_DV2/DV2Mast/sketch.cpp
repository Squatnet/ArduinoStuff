#include <FastLED.h>
#include <Arduino.h>
#include <PJON.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
uint8_t bus_id[4] = {0, 0, 1, 53}; // aNCS Unique Bus ID :)
PJON<SoftwareBitBang> bus(bus_id, 1); // MASTER SO ID 1
SoftwareSerial hc05(10, 11); // rx, tx
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
/* void compressStr(){
  for(int i=0;i<=numStrip;i++){
    if (strips[i].id = NULL){
            
    }
  }
}; */
void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if (code == PJON_CONNECTION_LOST) {
    Serial.print("Connection with device ID ");
    Serial.print(bus.packets[data].content[0], DEC);
    Serial.println(" is lost.");
  }
  if (code == PJON_PACKETS_BUFFER_FULL) {
    Serial.print("Packet buffer is full, has now a length of ");
    Serial.println(data, DEC);
    Serial.println("Possible wrong bus configuration!");
    Serial.println("higher PJON_MAX_PACKETS if necessary.");
  }
  if (code == PJON_CONTENT_TOO_LONG) {
    Serial.print("Content is too long, length: ");
    Serial.println(data);
  }
};
int findDeviceByName(char *type, char *nme){
  const char * ffs = type;
  const char * grr = nme;
  String fStr = String(ffs);
  String gStr = String(grr);
 if (fStr.compareTo("S") == 0){
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
  else if (fStr.compareTo("M") == 0){
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
  else if (fStr.compareTo("T") == 0){
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
  else if (fStr.compareTo("R") == 0){
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
void parseMsg(int id, char *msg) {
  Serial.println("#msgparser");
  Serial.println(msg);
  if (msg[0] == '{') {
    Serial.print("Json identified ");
    Serial.println(msg);
    const size_t bufferSize = JSON_OBJECT_SIZE(20) + 370;
    DynamicJsonBuffer jsonBuffer(bufferSize);
    JsonObject& json = jsonBuffer.parseObject(msg);
    Serial.print("#ParseJson ");
    if (json.success()) {
      Serial.println("PARSED SUCCESS");
      json.printTo(Serial);
      if (json.containsKey("register")) {
        Serial.println("GOT TYPE : REGISTERING");
        if (json["register"] == "strip") {
          strips[numStrip].id = id;
          const char* nameFFS = json["name"];
          Serial.print("THE DATA IS ");
          Serial.print(nameFFS);
          strips[numStrip].namee = String(nameFFS);
          Serial.print("Strip : ");
          Serial.print(numStrip);
          Serial.print(" name : ");
          Serial.print(strips[numStrip].namee);
          Serial.print(" id : ");
          Serial.println(strips[numStrip].id);
          numStrip++;
        }
        else if (json["register"] == "matrix") {
          matrix[numMatrix].id = id;
          const char* nameFFS = json["name"];
          Serial.print("THE DATA IS ");
          Serial.print(nameFFS);
          matrix[numMatrix].namee = String(nameFFS);
          Serial.print("Matrix : ");
          Serial.print(numMatrix);
          Serial.print(" name : ");
          Serial.print(matrix[numMatrix].namee);
          Serial.print(" id : ");
          Serial.println(matrix[numMatrix].id);
          numMatrix++;
        }
        else if (json["register"] == "terminal") {
          term[numTerm].id = id;
          const char* nameFFS = json["name"];
          Serial.print("THE DATA IS ");
          Serial.print(nameFFS);
          term[numTerm].namee = String(nameFFS);
          Serial.print("Terminal : ");
          Serial.print(numTerm);
          Serial.print(" name : ");
          Serial.print(term[numTerm].namee);
          Serial.print(" id : ");
          Serial.println(term[numTerm].id);
          numTerm++;
        }
        else if (json["register"] == "wnout") {
          wizNetOut.id = id;
          const char* nameFFS = json["name"];
          Serial.print("THE DATA IS ");
          Serial.print(nameFFS);
          wizNetOut.namee = String(nameFFS);
          Serial.print("Wn Out : ");
          Serial.print(" name : ");
          Serial.print(wizNetOut.namee);
          Serial.print(" id : ");
          Serial.println(wizNetOut.id);
        }
        else if (json["register"] == "wnin") {
          wizNetIn.id = id;
          const char* nameFFS = json["name"];
          Serial.print("THE DATA IS ");
          Serial.print(nameFFS);
          wizNetIn.namee = String(nameFFS);
          Serial.print("Wn Out : ");
          Serial.print(" name : ");
          Serial.print(wizNetIn.namee);
          Serial.print(" id : ");
          Serial.println(wizNetIn.id);
        }  x 
        else Serial.println("unknown type");
      }
      else if (json.containsKey("find")){
        int devId = findDeviceByName(json["type"],json["find"]);
        Serial.print("Found device with id ");
        Serial.println(devId);
      }
    }
    else Serial.print("Parse Faile");
  }
  else {
    Serial.println(msg);
    Serial.println("WHAT?");
  }
}
void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  const char * arr = payload;
  int id = int(packet_info.sender_id);
  parseMsg(id, arr);
  Serial.println(arr);
}

void printStrips(){
  if (debugMode == 1){
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
void setup() {
  Serial.begin(115200);
  hc05.begin(38400);
  Serial.print("Setup");
  bus.strategy.set_pin(12);
  bus.set_receiver(receiver_function);
  bus.set_error(error_handler);
  bus.begin();
}
void loop() {
  bus.receive(1000);
  bus.update();
  if (hc05.available()) {
    String str = "";
    Serial.print("got Blleeeeeeeeeuetotth  ");
    while ( hc05.available()) {
      char c = hc05.read();
      str.concat(c);
    }

    if (str != "") {
      const char buff[str.length() + 1];
      Serial.println(str);
      str.toCharArray(buff, str.length() + 1);
      Serial.println("sending to parser");
      parseMsg(int(99), buff);
      str = "";
    }
  }
  EVERY_N_SECONDS(5){ printStrips();}
  if (Serial.available())hc05.write(Serial.read());
}
