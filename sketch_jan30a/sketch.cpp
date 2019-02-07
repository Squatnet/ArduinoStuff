#include <Arduino.h>
long i = 0;
String st = "fuckyeah";
int in = 4;
float fl = 3.56;
int parser(String x){
  // only does string
  Serial.println("1 arg");
  };
int parser(String x, int y){
  Serial.println("2 arg");
  };
int parser(String x, int y, float z){
  Serial.println("3 arg");
  };

void setup() {
  Serial.begin(115200);
  parser(st);
  parser(st,in);
  parser(st,in,fl);
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}
