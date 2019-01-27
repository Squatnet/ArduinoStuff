void setup() {
  // put your setup code here, to run once:
 const char * naMe = "fucknugget";
 String nameStr = "";
 Serial.begin(115200);
 nameStr.concat(naMe);
 Serial.println(nameStr);
 String inTest = "No Ints";
 String inTest2 = "Some1nt5";
 String inTest3 = "1234";
 Serial.println("Int test");
 Serial.print(inTest);
 Serial.print(" ");
 Serial.println(inTest.toInt());
  Serial.println("Int test 2");
 Serial.print(inTest2);
 Serial.print(" ");
 Serial.println(inTest2.toInt());
  Serial.println("Int test 3");
 Serial.print(inTest3);
 Serial.print(" ");
 Serial.println(inTest3.toInt());
}

void loop() {
  // put your main code here, to run repeatedly:

}
