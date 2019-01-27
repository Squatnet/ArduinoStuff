// Varidaic Debug Macro
#define DEBUG   //Comment this line to disable Debug output
#ifdef DEBUG    // Debug is on
  #define DPRINT(...)    Serial.print(__VA_ARGS__)     //Sends our arguments to DPRINT()
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //Sends our arguments to DPRINTLN()
  #define DFLUSH(...)    Serial.flush()
#else // Debug is off
  #define DPRINT(...)     //Nothing Happens
  #define DPRINTLN(...)   //Nothing Happens
  #define DFLUSH(...)
#endif // end macro
void(* resetFunc) (void) = 0; // Software reset hack
String toParse = "Dev,23,Ptn,34,Color,0,Atm,1,Ats,13,Msg,ANCS!!,"; 
String toParse2 = "Ptn,34,Color,Atm,1,Ats,13,FF00FF,Msg,ANCS!!,"; 
String toParse3 = "Dev,23,Ptn,34,Atm,1,Ats,13,Msg,ANCS!!,Color,FF00FF,"; 
String toParse4 = "Color,FF00FF,Atm,1,Dev,23,Ptn,34,Ats,13,Msg,ANCS!!"; 
void parser(String string){
  while(string.length() >= 1){ // While there message left to read. 
    DPRINT("Message is ");
    DPRINT(string);
    DPRINT(" With length ");
    DPRINTLN(string.length());
    DPRINT("Next ',' is at ");
    DPRINTLN(string.indexOf(","));
    //if (string.indexOf(','))
    String subs = string.substring(0,string.indexOf(","));
    DPRINT("Substring was : ");
    DPRINT(subs);// get everything until the first comma.
    string.remove(0,string.indexOf(",")+1); // remove everything up to and including the first comma
    DPRINT(" - Message is now : ");
    DPRINT(string);
    DPRINT(" With length : ");
    DPRINTLN(string.length());
    if (subs.startsWith("Rst"))resetFunc(); // Reboot yourself. messge is destryed at this point
    else if (subs.startsWith("Ptn")){ // next value is pattern. 
      String ptn = string.substring(0,string.indexOf(",")); // get everything until the comma
      DPRINT("Pattern :");
      DPRINTLN(ptn);// Its going to be an integer. its the pattern number,
      string.remove(0,string.indexOf(",")+1); // Remove the value
    }
    else if (subs.startsWith("Dev")){ // random addition 
      String dev = string.substring(0,string.indexOf(",")); // get everything until the comma
      DPRINT("Device :");
      DPRINTLN(dev);// Its going to be an integer. its the pattern number,
      string.remove(0,string.indexOf(",")+1); // Remove the value
    }
    else if (subs.startsWith("Atm")){ // next value is boolean for automode
      String atm = string.substring(0,string.indexOf(",")); // get until first comma
      DPRINT("AutoMode :");
      DPRINTLN(atm); // also an integer
      string.remove(0,string.indexOf(",")+1); // remove it
    }
    else if (subs.startsWith("Ats")){ // next value is autoSecs
      String ats = string.substring(0,string.indexOf(",")); // get the value, 
      DPRINT("AutoSecs :");
      DPRINTLN(ats);
      string.remove(0,string.indexOf(",")+1); // remove the value and trailing comma
    }
    else if (subs.startsWith("Col")){ // its the color
      DPRINT("COLOR : r:");
      String r = string.substring(0,string.indexOf(",")); // first bit is red, 
      DPRINT(r);
      DPRINT(" ");// convert to an int
      string.remove(0,string.indexOf(",")+1); // remove red and comma
      String b = string.substring(0,string.indexOf(",")); // next up its blue
       DPRINT(b);
      DPRINT(" ");; // to integer
      string.remove(0,string.indexOf(",")+1); // remove blue and comma
      String g = string.substring(0,string.indexOf(",")); // then green 
       DPRINT(g);
      DPRINT(" ");
      string.remove(0,string.indexOf(",")+1); // thats colour done, remove the value and the comma
    }
    else { 
      string.remove(0,string.indexOf(",")+1);
    DPRINTLN(string);
    DPRINTLN(string.length());
    }// prints the length of the command each iteration
  }
  DPRINT("STR = "); // prints after length < 1
  DPRINTLN(string);
  string = ""; // empty it
}
void setup() {
  // put your setup code here, to run once:
 Serial.begin(115200);
 parser(toParse);
}

void loop() {
  // put your main code here, to run repeatedly:

}
