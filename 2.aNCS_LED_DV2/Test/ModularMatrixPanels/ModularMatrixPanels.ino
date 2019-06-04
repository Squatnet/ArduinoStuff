/* VARIDIC DEBUG MACRO */

#define DEBUG   //Comment this line to disable Debug output
#ifdef DEBUG    // Debug is on
#define DBEGIN(...)    Serial.begin(__VA_ARGS__)
#define DPRINT(...)    Serial.print(__VA_ARGS__)     //Sends our arguments to DPRINT()
#define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //Sends our arguments to DPRINTLN()
#define DFLUSH(...)    Serial.flush()

#else // Debug is off
#define DBEGIN(...)
#define DPRINT(...)     //Nothing Happens
#define DPRINTLN(...)   //Nothing Happens
#define DFLUSH(...)

#endif // end macro
/* VARIDIC DEBUG MACRO ENDS */
/* INCLUDES AND DEBUG LED */
#include <avr/eeprom.h> // allows using read_block and write_block
#include <EEPROM.h>
#define DEBUG_LED 13
bool debugLED = true;
uint16_t ledTimeout = 400;
uint16_t ledCurrMS = 0;
uint16_t lastMS;
int ledFlashesLeft = 0;
/* Define the data structure and create one called devInfo */
struct devStruct{
  char *dType;
  char *dName;
  int dNumPanels;
}devInfo;

String string; // holds serial in
void(* resetFunc) (void) = 0; // Software reset hack

/* Flash The LED and Delay */
void flashLed(){
  /* 
  DPRINT("C ");
  DPRINT(ledCurrMS);
  DPRINT(" L ");
  DPRINT(lastMS);
  DPRINT(" T ");
  DPRINT(ledTimeout);
  DPRINT(" F ");
  DPRINT(ledFlashesLeft);
  DPRINT(" - ");
  DPRINTLN(ledCurrMS-lastMS);
  */
  if (ledFlashesLeft > 0){
    if ( (ledCurrMS-lastMS) > ledTimeout){
      DPRINTLN("Flash");
      DFLUSH();
      lastMS = millis();
      ledFlashesLeft--;
      DPRINTLN(ledFlashesLeft);
        if(!debugLED){
          debugLED = true;
          digitalWrite(DEBUG_LED,HIGH);
        }
        else{
          debugLED = false;
          digitalWrite(DEBUG_LED,LOW);
        }
      }
  }
  else{
      debugLED = false;
      digitalWrite(DEBUG_LED,LOW);
  }
}
void eepromPut(char * dtype, char * dname, int numPanels){
  DPRINT(dtype);
  DPRINT(" ");
  DPRINT(dname);
  DPRINT(" ");
  DPRINTLN(numPanels);
  devStruct thisDev = {
    dtype,
    dname,
    numPanels
  };
  DPRINTLN("Saving thisDev to EEPROM");
  DPRINT("GOT TYPE ");
  DPRINTLN(thisDev.dType);
  DPRINT("GOT NAME ");
  DPRINTLN(thisDev.dName);
  DPRINT("GOT NUMPANELS ");
  DPRINTLN(thisDev.dNumPanels);
  eeprom_write_block((const void*)&thisDev, (void*)0, sizeof(thisDev)+1);
  //EEPROM.put(0,thisDev);
  ledFlashesLeft = 6;
  ledTimeout = 6000;
}
void eepromGet(){
  int readByte = EEPROM.read(0);
  if(readByte == 255 or readByte == 0){
    DPRINTLN("EEPROM EMPTY");
    ledFlashesLeft = 4;
    ledTimeout = 3000;
  }
  else{
    eeprom_read_block((void*)&devInfo, (void*)0, sizeof(devInfo));
    //EEPROM.get(0,devInfo);
    DPRINTLN("Got Data from EEPROM");
    DPRINTLN(devInfo.dType);
    DPRINTLN(devInfo.dName);
    DPRINTLN(devInfo.dNumPanels);
    ledFlashesLeft = 2;
    ledTimeout = 3000;
  }
}
void eepromReadAll(){
    for (int i = 0 ; i < EEPROM.length() ; i++) {
      
    DPRINT(char(EEPROM.read(i)));
    }
    DPRINTLN();
}
void eepromClear(){
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  
  DPRINTLN("EEPROM CLEARED!");
}
void setup() {
  DBEGIN(115200);
  lastMS = millis();
  DPRINTLN("DONE");
  eepromGet();
}
void parser() {
  DPRINTLN("PARSER");
  byte randTrigger=0;//initializes a randPattern trigger (didn't need to be global);
  while (string.length() >= 1) { // While there message left to read.
    DPRINT("Message is ");
    DPRINT(string);
    DPRINT(" With length ");
    DPRINTLN(string.length());
    if (!string.endsWith(","))string.concat(","); // STOP CRASHING;
    String subs = string.substring(0, string.indexOf(',')); // get everything until the first comma.
    DPRINT("SubStr - ");
    DPRINT(subs);
    string.remove(0, string.indexOf(',') + 1); // remove everything up to and including the first comma
    DPRINT(" String - ");
    DPRINTLN(string);
    if (subs.startsWith("Rst"))resetFunc();
    if (subs.startsWith("Clr")) {
      DPRINTLN("Clear EEPROM");
      eepromClear();
      string = "";
    }
    if (subs.startsWith("All")){
      DPRINTLN("READING EEPROM BYTE BY BYTE");
      eepromReadAll();
      string = "";
    }
    if (subs.startsWith("Set")){
      String dTyp = string.substring(0, string.indexOf(",")); // get until first comma
      DPRINT("GOT TYPE ");
      DPRINTLN(dTyp);
      const char * dT = dTyp.c_str();
      DPRINTLN(dTyp.c_str());
      DPRINTLN(dT);
      string.remove(0, string.indexOf(",") + 1); // remove it
      String dNam = string.substring(0, string.indexOf(",")); // get until first comma
      DPRINT("GOT NAME ");
      DPRINTLN(dNam);
      const char * dN = dNam.c_str();
      DPRINTLN(dN);
      string.remove(0, string.indexOf(",") + 1); // remove it
      int dNumPan = string.substring(0, string.indexOf(",")).toInt(); // get until first comma
      string.remove(0, string.indexOf(",") + 1); // remove it
      DPRINT("GOT NumPans ");
      DPRINTLN(dNumPan);
      eepromPut(dT,dN,dNumPan);
      DPRINTLN("Saved EEPROM");
      delay(200);
      resetFunc();
    }
    if (subs.startsWith("Fla")){
      ledTimeout = string.substring(0, string.indexOf(",")).toInt(); // get until first comma
      string.remove(0, string.indexOf(",") + 1); // remove it
      ledFlashesLeft = string.substring(0, string.indexOf(",")).toInt();
      string.remove(0, string.indexOf(",") + 1); // remove it
    }
  }
};
void loop() {
  // put your main code here, to run repeatedly:
  ledCurrMS = millis();
  flashLed();
  if (Serial.available()){
    string = "";
    while (Serial.available()) {
      char c = Serial.read();
      string.concat(c);
      delay(5);
    }
    if (string != "") {
      parser();
      string = "";
    }
  }
}
