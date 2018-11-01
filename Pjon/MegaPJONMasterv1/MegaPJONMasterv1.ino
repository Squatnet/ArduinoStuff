#include <ArduinoJson.h>
/* SoftwareBitBang default SWBB_MODE: 1
   (Transfer speed: 16.949kBb or 2.11kB/s) */
#include <SoftwareSerial.h>
#include <PJONMaster.h>
#define SWBB_RESPONSE_TIMEOUT 3000 /* Synchronous acknowledgement response timeout*/
#define SWBB_BACK_OFF_DEGREE     4 // Set the back-off exponential degree (default 4)
#define SWBB_MAX_ATTEMPTS       20 // Set the maximum sending attempts (default 20)
uint8_t bus_id[4] = {0,0,1,53}; // aNCS Unique Bus ID :)
PJONMaster<SoftwareBitBang> bus(bus_id); // Correct initializer.
uint32_t t_millis;
String string = "";
const size_t bufferSize = 2*JSON_ARRAY_SIZE(4) + JSON_ARRAY_SIZE(7) + 15*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3);
DynamicJsonBuffer jsonBuffer(bufferSize);
SoftwareSerial BT(10,11);
JsonObject& root = jsonBuffer.createObject();
JsonArray& serial = root.createNestedArray("serial");
JsonArray& strips = root.createNestedArray("strips");
JsonArray& matrix = root.createNestedArray("matrix");
void setup() {
  BT.begin(38400);
  Serial.begin(115200);
  Serial.println("Starting Host BT Connection");
  bus.strategy.set_pin(12); //PJON 
  bus.debug = false;
  bus.set_receiver(receiver_function);
  bus.set_error(error_handler);
  bus.begin(); //PJON begin strategy
  t_millis = millis();
}
void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("PJON error: connection lost with device id ");
    Serial.println((uint8_t)bus.packets[data].content[0], DEC);
  }
  if(code == PJON_ID_ACQUISITION_FAIL) {
    Serial.print("PJONMaster error: connection lost with slave id ");
    Serial.println(data, DEC);
  }
  if(code == PJON_DEVICES_BUFFER_FULL) {
    Serial.print("PJONMaster error: master devices' buffer is full with a length of ");
    Serial.println(data);
  }
};
void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
     const char * arr = payload;
    // String rootStr = "";
  // Packet content
  Serial.print(" Packet: ");
  Serial.println(arr);
    msgParser(arr,packet_info.sender_id);
  
};
void msgParser(char *theMsg,uint8_t senderId){
    const char * arr = theMsg;
    if(arr[0]=='{'){
    const size_t bufferSize = JSON_OBJECT_SIZE(8);
    StaticJsonBuffer<bufferSize> ddBuffer;
    JsonObject& Msg = ddBuffer.parseObject(arr);
    if(Msg.success()){
      Serial.print("Parse Success");
      Msg.printTo(Serial);
      Serial.println();
      if( Msg.containsKey("register")){
        Serial.print("register key found : ");
        registerDevice(Msg,senderId);
       }
       else if (Msg.containsKey("poll")){
        Serial.print("Poll key found :");
        if(Msg["poll"].is<uint8_t>()){
          Msg["poll"].printTo(Serial);
          Serial.println(" ID");
          int tmp = Msg["poll"].as<uint8_t>();
          Serial.println(tmp);
          pollDevice(tmp);
        }
        else {
          Serial.println();
          DynamicJsonBuffer ddbuff;
          const JsonVariant& tmp =  getDeviceByName(Msg["poll"],Msg["type"]);
          const JsonArray& list = tmp.as<JsonArray>();
         for (auto value : list) {
          Serial.print("Polling device ");
          value.printTo(Serial);
          Serial.println();
          pollDevice(value);
         }
        }
       }
       else if (Msg.containsKey("pollreply")){
        Msg.printTo(Serial);
       }
       else if (Msg.containsKey("control")){
        Msg.printTo(Serial);
       }
    }
    else {
        Serial.print("JsonParseError");
      }
   
   ddBuffer.clear();
  }
  else Serial.print(arr);
  Serial.println("Finished PARSE");
  };
void registerDevice(JsonObject &Dev,uint8_t senderId){
  Dev["register"].printTo(Serial);
        Serial.println();
        if (Dev["register"] == "Strip"){
          Serial.print("Got Str");
          JsonObject& singleDvc = strips.createNestedObject();
          if(singleDvc.success()){
            Serial.print("Added");
            singleDvc["name"] = Dev["name"];
            singleDvc["id"] = senderId;
            singleDvc.printTo(Serial);
            strips.printTo(Serial);
          }
          else Serial.println("MEMORY ALLOCATION");
        }
        
        else if (Dev["register"] == "Serial"){
           Serial.print("Got Serial");
          JsonObject& singleDvc = strips.createNestedObject();
          if(singleDvc.success()){
            Serial.print("Added");
            singleDvc["name"] = Dev["name"];
            singleDvc["id"] = senderId;
            singleDvc.printTo(Serial);
            serial.printTo(Serial);
          }
          else Serial.println("MEMORY ALLOCATION");
        }
        else if (Dev["register"] == "Matrix"){
           Serial.print("Got Mat");
          JsonObject& singleDvc = strips.createNestedObject();
          if(singleDvc.success()){
            Serial.print("Added");
            singleDvc["name"] = Dev["name"];
            singleDvc["id"] = senderId;
            singleDvc.printTo(Serial);
            matrix.printTo(Serial);
          }
          else Serial.println("MEMORY ALLOCATION");
        }
        else {
          const char * v = Dev["register"];
          Serial.print(v);
          }
};
JsonVariant getDeviceByName(char *data,char *type){
  const size_t bufferSize = JSON_ARRAY_SIZE(38);
   DynamicJsonBuffer ArrBuffer(bufferSize);
  JsonArray& devices = ArrBuffer.createArray();
  if (type == "Strips"){
    for (auto value : strips) {
      const char * tmp = value;
      if(tmp == data)devices.add(value["id"]);
    }
  }
  else if (type == "Matrix"){
    for (auto value : strips) {
      if(value["name"] == data)devices.add(value["id"]);
    }
  }
  else if (type == "Serial"){
    for (auto value : strips) {
      if(value["name"] == data)devices.add(value["id"]);
    }
  }
  else if (type == "Router"){
   for (auto value : strips) {
      if(value["name"] == data)devices.add(value["id"]);
    }
  }
  else Serial.print("undefined type");
  devices.printTo(Serial);
  return devices;
}
void pollDevice(uint8_t rid){
  Serial.print("poll func - ID: ");
  Serial.println(rid);
  const char * JSON = "{\"poll\":\" \"";
    int Size = 0;
    while (JSON[Size] != '\0') Size++;
    Serial.print("Length - ");
    Serial.println(Size);
    Serial.print("response ");
    Serial.println(bus.send_packet(rid,"{\"poll\":\" \"",11));
    //if( == PJON_ACK)Serial.print("SENT");
}
void pollMany(JsonArray &arr){
  for (auto value : arr) {
    pollDevice(value);
}
}
void loop() {
  if(millis() - t_millis > 15000) {
    // Check if registered slaves are still present on the bus
    bus.check_slaves_presence();
    t_millis = millis();
    serial.printTo(Serial);
    Serial.println(" Serial");
    strips.printTo(Serial);
    Serial.println(" Strips");
    matrix.printTo(Serial);
    Serial.println(" Matrix");
  }
  bus.receive(5000);
  bus.update();
  string = "";
  while(BT.available()){
     char character = BT.read(); // Receive a single character from the software serial port
        string.concat(character); // Add the received character to the receive buffer
  }
  if (string != ""){
    if (string.startsWith("{")){
    char buffer[string.length()+1];
    string.toCharArray(buffer, string.length()+1);
    msgParser(buffer,999);
    }
    else BT.write("Please use JSON");
    string = "";
  }
}
void relayMsg(char name, char *Msg){
  //dothis
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
