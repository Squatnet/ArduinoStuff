// 022/0b - ProMini with HC05 and 3 LED MEnu options. PJon from pin 12

// Aim: Collect and pass string to network. Use 'special' commands to make menu selections. 

// DEBUG code! 

#include <SoftwareSerial.h>
#include <PJON.h>

#define fL(x,a) for(int x=0;x<a;x++)

SoftwareSerial BTSerial(10, 11); // RX | TX for HC05 Module

#define SWBB_RESPONSE_TIMEOUT 1500 /* Synchronous acknowledgement response timeout*/
#define SWBB_BACK_OFF_DEGREE     4 // Set the back-off exponential degree (default 4)
#define SWBB_MAX_ATTEMPTS       20 // Set the maximum sending attempts (default 20)

uint8_t bus_id[4] = {0,0,1,53}; // aNCS Unique Bus ID :)
PJON<SoftwareBitBang> bus(bus_id, 1); // Correct initializer.

String string = "";
char strS[28]; 

int leds[] = {2,3,4};
bool IO[3] = {0,0,0};

void menu(int a){
  if(IO[a]==1)IO[a]=0;
  else IO[a]=1;
  fL(i,3){ digitalWrite(leds[i] , IO[i]); }
} 

void setup() {
  Serial.begin(9600);
  Serial.println("Starting Host BT Connection");
  fL(b,3){ pinMode(leds[b], OUTPUT ); }
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
  string.toCharArray(strS, sizeof(strS));
  
  if (string.startsWith("L")){
    if(string.substring(1,2) == "A") { // A/B/C
      bus.send_packet(10, bus_id, "#1", 2); // send TFT recognition of Menu selection
      menu(0); // run menu LED     
    } 
    if(string.substring(1,2) == "B") { // A/B/C
      bus.send_packet(10, bus_id, "#2", 2); // send TFT recognition of Menu selection
      menu(1); // run menu LED     
    } 
    if(string.substring(1,2) == "C") { // A/B/C
      bus.send_packet(10, bus_id, "#3", 2); // send TFT recognition of Menu selection
      menu(2); // run menu LED     
    } 
  }

    if (string.startsWith("M")){
    if(string.substring(1,2) == "A") { // A/B/C
      bus.send_packet(30, bus_id, "MA", 2); // send R recognition of Menu selection       
    } 
    if(string.substring(1,2) == "B") { // A/B/C
      bus.send_packet(30, bus_id, "MB", 2); // send R recognition of Menu selection      
    } 
    if(string.substring(1,2) == "C") { // A/B/C
      bus.send_packet(30, bus_id, "MC", 2); // send R recognition of Menu selection       
    } 
  }
  
  if (string != ""){
            Serial.println(string); //Output the message to serial
            //bus.send_packet(10, bus_id, "Hi!", 3);
            bus.send_packet(10, bus_id, strS, sizeof(strS)); // send the string to TFT
            //bus.send_packet(30, bus_id, strS, sizeof(strS)); // send the string to 022_R
            string =""; //clear the buffer/message
        }
 bus.update(); // update and prepare to send in loop
  if (Serial.available())
    BTSerial.write(Serial.read());
}


