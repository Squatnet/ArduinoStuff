#include <SoftwareSerial.h>

//Create software serial object to communicate with SIM800L
SoftwareSerial sim(D2, D3, false, 256); //SIM800L Tx & Rx is connected to Arduino #3 & #2
String simInBuff = "";
void setup()
{
  delay(5000);
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(115200);
  delay(100);
  Serial.flush();
  //Begin serial communication with Arduino and SIM800l
  Serial.println("Initializing...");
  delay(100);
  Serial.flush();
  delay(5000);
  sim.begin(115200);
  simSetup();
  pinMode(16, OUTPUT);
  digitalWrite(16,HIGH);
  sendSMS("+447428981160","Completed Setup");
}
void simSetup(){
  Serial.println("Sim800 initialising");
  delay(100);
  pinMode(D4,OUTPUT);
  delay(300);
  pinMode(D4,INPUT);
  for(int i=0;i<30;i++){
    Serial.print(" .");
    delay(200 );
  }
  sim.println();
  sim.println("AT");
  delay(100);
  updateSerial();
  sim.println("AT");
  delay(100);
  updateSerial();
  sim.println("AT");
  delay(100);
  updateSerial();
  sim.println("AT");
  delay(100);
  updateSerial();
  sim.println("ATI");
  delay(100);
  updateSerial();
  Serial.print("Sim ID : ");
  sim.println("AT+CCID");
  delay(100);
  updateSerial();
  Serial.print("Network info : ");
  sim.println("AT+COPS?");
  delay(100);
  updateSerial();
  Serial.print("Set sim800 to TEXT mode : ");
  sim.println("AT+CMGF=1");
  delay(100);
  updateSerial();
  Serial.print("SMS Settings (0,2,0,0,0) :");
  sim.println("AT+CNMI=2,1,0,0,0");
  delay(100);
  updateSerial();
  Serial.print("SMS Settings (1,2,0,0,0) :");
  sim.println("AT+CPMS=\"ME\",\"ME\",\"ME\"");
  delay(100);
  updateSerial();
  Serial.print("List all SMS messages");
  sim.println("AT+CMGL=\"ALL\"");
  delay(100);
  updateSerial();
  Serial.print("waiting for SMS list");
  delay(1000);
  updateSerial();
  Serial.print("Delete all SMS messages");
  sim.println("AT+CMGD=?");
  delay(1000);
  updateSerial();
  Serial.print("List all SMS messages");
  sim.println("AT+CMGL=\"ALL\"");
  delay(100);
  updateSerial();
}
void sendSMS(String num, String mess){
  Serial.print("Sending : ");
  Serial.print(mess);
  Serial.print(" To : ");
  Serial.println(num);
  String toSend = "AT+CMGS=\"";
  toSend.concat(num);
  toSend.concat("\"");
  sim.println(toSend);
  updateSerial();
  sim.print(mess);
  updateSerial();
  sim.write(26);
  updateSerial();
}
void loop()
{
  updateSerial();
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    sim.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  if(sim.available()){
    while(sim.available()){
      char c = sim.read();
      simInBuff.concat(c);
    }
    Serial.println(simInBuff);
    if (simInBuff.indexOf("ERROR") != -1){
      pinMode(D4,OUTPUT);
      delay(300);
      pinMode(D4,INPUT);
      delay(300);
      Serial.flush();
      delay(2000);
    }
    if (simInBuff.indexOf("+CMT") != -1){
      Serial.println("GOT SMS");
    }
    simInBuff = "";
   
  }
}
