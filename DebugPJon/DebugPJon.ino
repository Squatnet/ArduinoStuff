#include <PJON.h>

uint8_t bus_id[] = {0, 0, 1, 53}; //  BACK to our Unique ID. One NETWORK , ONE ID - Making ANCS GREAT AGAIN
PJON<SoftwareBitBang> bus(bus_id, 32);

void setup() {
 bus.strategy.set_pin(12);
  bus.begin();
  
}

void loop() {
  bus.send_packet(10,"2", 2, 1000000);
  bus.update();
}
