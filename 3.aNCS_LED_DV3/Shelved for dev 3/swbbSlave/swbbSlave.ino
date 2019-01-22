#include <PJONSlave.h> 
uint8_t bus_id[] = {0, 0, 1, 53};
PJONSlave<SoftwareBitBang> bus(bus_id, PJON_NOT_ASSIGNED); // Force no id... 
int packet;
bool acquired = false; // did we get an address? 
bool debugMode = false;
uint32_t t_millis;
int ourID = 255;
void(* resetFunc) (void) = 0;
void receiver_handler(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  Serial.print("Received: ");
  for(uint16_t i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.flush();
};

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection lost with device ");
    Serial.println((uint8_t)bus.packets[data].content[0], DEC);
    delay(1000);
    resetFunc();
  }
  if(code == PJON_ID_ACQUISITION_FAIL) {
    if(data == PJON_ID_ACQUIRE) {
      Serial.println("PJONSlave error: multi-master addressing failed.");
      delay(1000);
      resetFunc();
    }
    if(data == PJON_ID_CONFIRM)
      Serial.println("PJONSlave error: master-slave id confirmation failed.");
    if(data == PJON_ID_NEGATE)
      Serial.println("PJONSlave error: master-slave id release failed.");
    if(data == PJON_ID_REQUEST)
      Serial.println("PJONSlave error: master-slave id request failed.");
      delay(400);
      if (millis() > 15000){
        Serial.println("Resetting due to no ID");
        delay(300);
        resetFunc();
        }
      bus.acquire_id_master_slave();
      acquired = false;
      delay(160);
     
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
void tellMasterAboutSelf(){
  String newPkt = "Reg,Str,Any";
  const char pkt[newPkt.length()+1];
  newPkt.toCharArray(pkt,newPkt.length());
  bus.send_packet(254,pkt,newPkt.length()+1);
}
void loop() {
  if(ourID == 255 && millis() > 15000){
    Serial.println("NO ID AFTER 15000");
    resetFunc();
  }
  if((bus.device_id() != PJON_NOT_ASSIGNED) && !acquired) {
    Serial.print("Acquired device id: ");
    Serial.println(bus.device_id());
    Serial.flush();
    delay(100);
    acquired = true;
    tellMasterAboutSelf();
  }
  bus.update();
  bus.receive(5000);
}
