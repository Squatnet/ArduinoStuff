// 022_R Router With PJON on 12 and 7, 3 analog pots and 1x 74hc595 LED array.

// Aim : to Receive pulse signals and relay messages to "branch" of lights and relay board

#define LPIN 4 // Latch
#define CPIN 5 // Clock
#define DPIN 3 // Data

int aPins[] = {0,1,2};
int val[] = {0,0,0};

byte data; // 595 Byte data interger (Bits)
byte dataArray[11]; //LED Array Type 1

void aR(int bb){
  val[bb] = analogRead(bb);
  val[bb] = map(val[bb], 0, 1023, 0, 255);
  Serial.println(val[bb]);
}

void hc595(int aa) {
  data = dataArray[aa];
        digitalWrite(LPIN, 0);
        shiftOut(DPIN, CPIN, data);
        digitalWrite(LPIN, 1);
}
void setup() {
  pinMode(LPIN, OUTPUT); // Init 595 (menu)
  dataArray[0] = 0b00000000;
  dataArray[1] = 0b11111100;
  dataArray[2] = 0b11110011;
  dataArray[3] = 0b00100000;
  dataArray[4] = 0b00010000;
  dataArray[5] = 0b00001000;
  dataArray[6] = 0b00000100;
  dataArray[7] = 0b00000010;
  dataArray[8] = 0b00000001;
  dataArray[9] = 0b11111111;
  dataArray[10] = 0b10101010;
}

void loop() {
  // put your main code here, to run repeatedly:

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
