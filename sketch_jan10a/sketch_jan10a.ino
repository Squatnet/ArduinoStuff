void(* resetFunc) (void) = 0; //declare reset function @ address 0
void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
Serial.println("SETUP RUNS");
delay(500);
Serial.println("Resetting");
resetFunc();
Serial.println("Never prints");
}
void loop() {
  // put your main code here, to run repeatedly:

}
