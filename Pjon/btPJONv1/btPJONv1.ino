#include <ArduinoJson.h>


/* SoftwareBitBang default SWBB_MODE: 1
   (Transfer speed: 16.949kBb or 2.11kB/s) */
#include <SoftwareSerial.h>
#include <PJONMaster.h>
SoftwareSerial BTSerial(9, 8); // RX | TX for HC05 Module

#define SWBB_RESPONSE_TIMEOUT 1500 /* Synchronous acknowledgement response timeout*/
#define SWBB_BACK_OFF_DEGREE     4 // Set the back-off exponential degree (default 4)
#define SWBB_MAX_ATTEMPTS       20 // Set the maximum sending attempts (default 20)
uint8_t bus_id[4] = {0,0,1,53}; // aNCS Unique Bus ID :)
PJONMaster<SoftwareBitBang> bus(bus_id); // Correct initializer.
uint32_t t_millis;
String string = "";
const size_t bufferSize = JSON_ARRAY_SIZE(3) + 2*JSON_ARRAY_SIZE(5) + JSON_OBJECT_SIZE(3)+200;
DynamicJsonBuffer jsonBuffer(bufferSize);

JsonObject& root = jsonBuffer.createObject();
JsonArray& serial = root.createNestedArray("serial");
JsonArray& strips = root.createNestedArray("strips");
JsonArray& matrix = root.createNestedArray("matrix");
void setup() {
  Serial.begin(115200);
  Serial.println("Starting Host BT Connection");
  BTSerial.begin(38400); //HC05
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
     String rootStr = "";
  // Packet content
  Serial.print(" Packet: ");
  Serial.print(arr);
  if(arr[0]=='{'){
    const size_t bufferSize = JSON_ARRAY_SIZE(3) + 2*JSON_ARRAY_SIZE(5) + JSON_OBJECT_SIZE(3);
    DynamicJsonBuffer ddBuffer(bufferSize);
    JsonObject& Msg = ddBuffer.parseObject(arr);
    if(Msg.success()){
      if( Msg.containsKey("type")){
        JsonObject& singleDvc = ddBuffer.createObject();
        singleDvc["name"] = Msg["name"];
        singleDvc["id"] = packet_info.sender_id;
        singleDvc.printTo(Serial);
        if (Msg["type"] == "Strip")strips.add(singleDvc);
        else if (Msg["type"] == "Serial")serial.add(singleDvc);
        else if (Msg["type"] == "Matrix")matrix.add(singleDvc);
        else {
          const char * v = Msg["type"];
          Serial.print(v);
          }
       }
    }
    else {
        Serial.print("JsonParseError");
      }
  }
  else Serial.print(arr);
};
void loop() {
  if(millis() - t_millis > 5000) {
    // Check if registered slaves are still present on the bus
    bus.check_slaves_presence();
    Serial.println("List of slaves known by master: ");
    for(uint8_t i = 0; i < PJON_MAX_DEVICES; i++) {
      if(bus.ids[i].state) {
        Serial.print(" - Device id: ");
        Serial.print(i + 1); // Shifted by one to avoid PJON_BROADCAST
        Serial.print(" Device rid: ");
        Serial.print(bus.ids[i].rid);
        Serial.println();
      } 
    }
    Serial.println();
    Serial.flush();
    t_millis = millis();
  }
  bus.receive(5000);
  bus.update();
  if(BTSerial.available()){
    while(BTSerial.available()){
    char character = BTSerial.read(); // Receive a single character from the software serial port
    string.concat(character); // Add the received character to the receive buffer
    }
  string.trim();
  String temp = string.substring(3);
  char buff[temp.length()+1];
  temp.toCharArray(buff, sizeof(buff));
  for (auto value : serial) {
  int id = value["id"];
  bus.send_packet(id, bus_id, buff, sizeof(buff));    }
  if (string.startsWith("LED")){
    if(string.substring(3,4) == "X"){
      for (auto value : strips) {
      int id = value["id"];
      bus.send_packet(id, bus_id, buff, sizeof(buff));
      }
    }
    else{
      for (auto value : strips) {
      if(value["name"] == string.substring(3,4)){
        int id = value["id"];
         bus.send_packet(id, bus_id, buff, sizeof(buff));
      }
    
      }
    }
  }
  if (string != ""){
            Serial.println(string); //Output the message
            //bus.send_packet(10, bus_id, "Hi!", 3);
           // bus.send_packet(10, bus_id, string, sizeof(string)+1);
           string =""; //clear the buffer/message
        }
// Keep reading from Arduino Serial Monitor and send to HC-05
  }
  if (Serial.available()){
    BTSerial.write(Serial.read());
}
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
