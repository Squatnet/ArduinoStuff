#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

  unsigned long mainDisplayDelay=2000; //milliseconds between switching display output 2 seconds.
  unsigned long interuptChangeDelay=50; //milliseconds to count since last change.
  unsigned long mainDelayStart=0; //will store time since last change to the main menue.
  unsigned long interuptDelayStart=0; //will store time since last change to interupter.
  bool mainDelayChange=false;//flag to see if the main delay is active.
  bool interuptChange=false; //flag to see if the interupt script is running.
  String messageOut; //used to concatinate main menue output string.
  String switches0; //as above for switch 1.
  String switches1; //as above for switch 2.
  int mainDisplayCounter=0; //used to keep track of menue display.
  int interuptNo=0; //used to keep track of which input has changed.
  // arrays used to track values of inputs, map them and compare them to a previous value.
  int dataIn[4]={0,0,0,0};
  int dataMap[4]={0,0,0,0};
  int memory[4]={0,0,0,0};
  
// Debugger as a Variadic Macro.
#define DEBUG   //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.
#ifdef DEBUG    //Macros are usually in all capital letters.
  #define DPRINT(...)    Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //DPRINTLN is a macro, debug print with new line
#else
  #define DPRINT(...)     //now defines a blank line
  #define DPRINTLN(...)   //now defines a blank line
#endif
  
void readPins(){//takes the current value of inputs then maps and stores them.
    for (int i=0;i<=3;i++){
    dataIn[i]=analogRead(i);
    }

  dataMap[0]=(map(dataIn[0],0,1024,0,2));
  dataMap[1]=(map(dataIn[1],0,1024,0,2));
  dataMap[2]=(map(dataIn[2],0,1024,0,128));
  dataMap[3]=(map(dataIn[3],0,1024,0,128));
  
  //creates the strings for the switches.
  if(dataMap[0]>=1){
    switches0=("On");}
    else{
    switches0=("Off");}
    
    if (dataMap[1]>=1){
    switches1=("On");}
    else{
    switches1=("Off");}
  
// checks if there is a change between an input and its memory and sets a timer.
  for (int i=0;i<=3;i++){
    if (memory[i]!=dataMap[i]){
      interuptChange=true;
      interuptDelayStart=millis();
      interuptNo=i;
      memory[i]=dataMap[i];
    }
  }
}

void mainDisplay(){
  readPins();
  changeDetector();
  if ((millis()-mainDelayStart)>=mainDisplayDelay){
    mainDelayStart+=mainDisplayDelay;
    mainDisplayCounter++;
    mainDelayChange=true;
    }
    
  if (mainDisplayCounter>=2){mainDisplayCounter=0;}
  
  if ((mainDisplayCounter==0)&&(mainDelayChange==true)&&(interuptChange==false)){
    messageOut=String("Switch 1 = ")+(switches0);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(messageOut);
    DPRINTLN("catch case 1");
    messageOut=String("");
  
    messageOut=String("Switch 2 = ")+(switches1);
    lcd.setCursor(0,1);
    lcd.print(messageOut);
    DPRINTLN("catch case 2");
    messageOut=String("");
    mainDelayChange=false; 
    }
  
    if ((mainDisplayCounter==1)&&(mainDelayChange==true)&&(interuptChange==false)){
    messageOut=String("Pot 1 = ")+(dataMap[2]);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(messageOut);
    DPRINTLN("catch case 3");
    messageOut=String("");
      
    messageOut=String("Pot 2 = ")+(dataMap[3]);
    lcd.setCursor(0,1);
    lcd.print(messageOut);
    DPRINTLN("catch case 4");
    messageOut=String("");
    mainDelayChange=false;
  }
}

void changeDetector(){
  if(interuptChange==true){
    
    if (interuptNo==0){
      messageOut=String("Switch 1 =");
    }
    else if (interuptNo==1){
      messageOut=String("Switch 2 =");
    }
    else if (interuptNo==2){
      messageOut=String("Pot 1 =");
    }
    else if (interuptNo==3){
      messageOut=String("Pot 2 =");
    }
  if ((millis()-interuptDelayStart)>=interuptChangeDelay){
    lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(messageOut);
      lcd.setCursor(0,1);
    if (interuptNo==0){
      lcd.print(switches0);
      delay(50);
    }
    else if (interuptNo==1){
      lcd.print(switches1);
      delay(50);
    }
    else if ((interuptNo==2)|(interuptNo==3)){
      lcd.print(dataMap[interuptNo]);
      delay(50);
    }
    messageOut=String("");
  }
  //turns off the change detector and resets the main menue timer and position
    if ((millis()-interuptDelayStart)>=mainDisplayDelay){
      interuptChange=false;
      mainDelayStart=millis();
      mainDisplayCounter=0;
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  #ifdef DEBUG
  Serial.begin(9600);
  DPRINTLN("setup begin");
  #endif
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  //starts the timers.
  mainDelayStart=millis();
  interuptDelayStart=millis();
  interuptChange=false;
  //pin declaration setup.
    for (int i=0;i<= 3;i++){
    char b=('A'+i);
    pinMode(i,INPUT);
    DPRINTLN ((i)+String(" is set to input"));
    }
  //set the memory to initial values
      memory[0]=(map(analogRead(0),0,1024,0,2));
      memory[1]=(map(analogRead(1),0,1024,0,2));
      memory[2]=(map(analogRead(2),0,1024,0,128));
      memory[3]=(map(analogRead(3),0,1024,0,128));
    DPRINTLN("initial memory set");
}

void loop() {
  // put your main code here, to run repeatedly:
  mainDisplay();
}
