int timer = 0; // resets at 64
int ratio = 1;
int i,b;

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
}

void loop() {
  timer ++;
  if (timer >=64)timer = 0;
  if (timer == 0){
    Serial.print("Timer Reset");
  }
   if (timer == 16){
    Serial.print("Timer Reset");
  }
   if (timer == 32){
    Serial.print("Timer Reset");
  }
   if (timer == 48){
    Serial.print("Timer Reset");
  }
}
