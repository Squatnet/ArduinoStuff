#include <PJONSlave.h> 
#define SWBB_RESPONSE_TIMEOUT 3000 /* Synchronous acknowledgement response timeout*/
#define SWBB_BACK_OFF_DEGREE     4 // Set the back-off exponential degree (default 4)
#define SWBB_MAX_ATTEMPTS       20 // Set the maximum sending attempts (default 20)
uint8_t bus_id[] = {0, 0, 1, 53};
PJONSlave<SoftwareBitBang> bus(bus_id, PJON_NOT_ASSIGNED); // Force no id... 
#include <ArduinoJson.h>
int packet;
bool acquired = false; // did we get an address? 
bool debugMode = false;
uint32_t t_millis;
void receiver_handler(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  const char * arr = payload;
     String rootStr = "";
  // Packet content
  Serial.print("Received: ");
  Serial.print(packet_info.sender_id);
  Serial.print(" Packet: ");
  Serial.print(arr);
  Serial.println();
  Serial.flush();
  if(arr[0] == '{'){
    const size_t bufferSize = JSON_OBJECT_SIZE(8);
    StaticJsonBuffer<bufferSize> ddBuffer;
    JsonObject& Msg = ddBuffer.parseObject(arr);
    if(Msg.success()){
      Serial.print("Parse Success");
      Msg.printTo(Serial);
      Serial.println();
      }
    else{
      Serial.println("Parse Failed");
    }
  }
};

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection lost with device ");
    Serial.println((uint8_t)bus.packets[data].content[0], DEC);
    autoReconnect();
  }
  if(code == PJON_ID_ACQUISITION_FAIL) {
    if(data == PJON_ID_ACQUIRE)
      Serial.println("PJONSlave error: multi-master addressing failed.");
    if(data == PJON_ID_CONFIRM)
      Serial.println("PJONSlave error: master-slave id confirmation failed.");
    if(data == PJON_ID_NEGATE)
      Serial.println("PJONSlave error: master-slave id release failed.");
    if(data == PJON_ID_REQUEST)
      Serial.println("PJONSlave error: master-slave id request failed.");
      delay(500);
      autoReconnect();
  }
  Serial.flush();
};

void setup() {
  Serial.begin(115200);
  bus.set_error(error_handler);
  bus.set_receiver(receiver_handler);
  bus.strategy.set_pin(12);
  bus.begin();
  delay(160); // possibly not needed if master is online
  bus.acquire_id_master_slave(); //get an id
}

void loop() {
  if((bus.device_id() != PJON_NOT_ASSIGNED) && !acquired) { 
    // we gto a new device id. 
    Serial.print("Acquired device id: ");
    Serial.println(bus.device_id());
    // we should tell master abut us... 
    tellMasterAboutSelf();
    Serial.flush();
    acquired = true;
  }
  bus.receive(5000);
  if (debugMode == true){
    if(millis() - t_millis > 5000) {
      tellMasterAboutSelf();
      t_millis = millis();
    }
  }
  yield();
  bus.update();
  
};
void tellMasterAboutSelf(){
  const char * JSON = "{\"register\":\"Strip\",\"name\":\"A\"}"; // enter the json to describe this slave here, note the \" to escape
  int Size = 0;
    while (JSON[Size] != '\0') Size++;
    if(bus.send_packet(254,JSON,Size) == PJON_ACK){
    Serial.print("Registered With Master");
    Serial.println(JSON);
   }
  else{
    bus.send_packet(254,JSON,Size);
  }
};
void respondToPoll(){
  Serial.print("Responding to poll request");
  String msg = "{\"pollreply\":\" \",\"pattern\":\"";
  msg += String(6);
  msg += "\",\"color\":[";
  msg += String(255);
  msg += ",";
  msg += String(255);
  msg += ",";
  msg += String(255);
  msg += ",\"automode\":";
  msg += String(1);
  msg += ",\"autosecs\":";
  msg += String(30);
  msg += "}";
  Serial.println(msg);
  char JSON[msg.length()+1];
  msg.toCharArray(JSON,msg.length());
    int Size = 0;
    while (JSON[Size] != '\0') Size++;
    int err = bus.send_packet(254,JSON,Size);
    Serial.print(err);
};
void autoReconnect(){
  acquired = false;
  bus.acquire_id_master_slave();
}
