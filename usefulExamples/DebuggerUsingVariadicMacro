#include<Wire.h>

#define DEBUG   //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.
#ifdef DEBUG    //Macros are usually in all capital letters.
  #define DPRINT(...)    Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //DPRINTLN is a macro, debug print with new line
#else
  #define DPRINT(...)     //now defines a blank line
  #define DPRINTLN(...)   //now defines a blank line
#endif

void setup(){
  Serial.begin(9600);
  //if DEBUG is not commented out.
  #ifdef DEBUG 
  DPRINTLN("Debugger Active");
  //if DEBUG is commented out.
  #else 
  Serial.println("Debugger Deactive");
  #endif
  DPRINTLN("setup complete");
    
}

void loop(){
delay(1000);
DPRINT("catch 1");
delay(1000);
DPRINTLN(" catch 2");
delay(1000);
DPRINTLN("catch 3");
}
