/* SoftwareBitBang default SWBB_MODE: 1
   (Transfer speed: 16.949kBb or 2.11kB/s) */
#include <SoftwareSerial.h>
#include <PJON.h>

SoftwareSerial BTSerial(9, 8); // RX | TX for HC05 Module

#define SWBB_RESPONSE_TIMEOUT 1500 /* Synchronous acknowledgement response timeout*/
#define SWBB_BACK_OFF_DEGREE     4 // Set the back-off exponential degree (default 4)
#define SWBB_MAX_ATTEMPTS       20 // Set the maximum sending attempts (default 20)
uint8_t bus_id[4] = {0,0,1,53}; // aNCS Unique Bus ID :)
PJON<SoftwareBitBang> bus(bus_id, 1); // Correct initializer.

String string = "";

#define LPIN 4 // Latch
#define CPIN 5 // Clock
#define DPIN 3 // Data
byte data; // 595 Byte data interger (Bits)
byte dataArray[11]; //LED Array Type 1
void hc595(int aa) {
  data = dataArray[aa];
        digitalWrite(LPIN, 0);
        shiftOut(DPIN, CPIN, data);
        digitalWrite(LPIN, 1);
        delay(50);
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
  Serial.begin(9600);
  Serial.println("Starting Host BT Connection");
  BTSerial.begin(38400); //HC05
  bus.strategy.set_pin(12); //PJON 
  bus.begin(); //PJON begin strategy
}

void loop() {
  while(BTSerial.available()){
     char character = BTSerial.read(); // Receive a single character from the software serial port
        string.concat(character); // Add the received character to the receive buffer
  }
  string.trim();
  if (string.startsWith("A")){
    bus.send_packet(10, bus_id, "A", 1);
    hc595(1);
   }
   if (string.startsWith("B")){
    bus.send_packet(10, bus_id, "B", 1);
    hc595(2);
   }
  if (string.startsWith("C")){
    bus.send_packet(10, bus_id, "C", 1);
    hc595(3);
   }
   if (string.startsWith("D")){
    bus.send_packet(10, bus_id, "D", 1);
    hc595(4);
   }
   if (string.startsWith("E")){
    bus.send_packet(10, bus_id, "E", 1);
    hc595(5);
   }
   if (string.startsWith("F")){
    bus.send_packet(10, bus_id, "F", 1);
    hc595(6);
   }
   if (string.startsWith("G")){
    bus.send_packet(10, bus_id, "G", 1);
    hc595(7);
   }
   if (string.startsWith("X")){
    bus.send_packet(10, bus_id, "X", 1);
    hc595(0);
   }
  if (string.startsWith("Y")){
    bus.send_packet(10, bus_id, "Y", 1);
    hc595(9);
   }
  if (string != ""){
            Serial.println(string); //Output the message
            //bus.send_packet(10, bus_id, "Hi!", 3);
           // bus.send_packet(10, bus_id, string, sizeof(string)+1);
           string =""; //clear the buffer/message
        }
 bus.update();
// Keep reading from Arduino Serial Monitor and send to HC-05
  if (Serial.available())
    BTSerial.write(Serial.read());
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
