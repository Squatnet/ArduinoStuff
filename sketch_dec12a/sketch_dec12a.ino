void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
String str = "FF12FF";
String str2 = "2340230";
String str3 = "forjsjd";
Serial.print(str.toInt());
Serial.print(" , ");
Serial.print(str2.toInt());
Serial.print(" , ");
Serial.print(str3.toInt());
Serial.print(" ! ");
}

void loop() {
  // put your main code here, to run repeatedly:

}
