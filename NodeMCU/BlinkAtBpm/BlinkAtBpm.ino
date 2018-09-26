/* 
 * Modified Blink script for NodeMCU  
 * Allows BPM to be input over serial
 * flashes 2 leds in time with BPM
 */
float bpm = 175; // sTarting BPM
String str = ""; // Empty String
int dLay = int(60000 / bpm); // Bpm > MS
void setup() {
  pinMode(13, OUTPUT); // RED LED
  pinMode(14, OUTPUT); // Yellow LED
  Serial.begin(9600); // Init Serial
}

// the loop function runs over and over again forever
void loop() {
  while(Serial.available()){ // While there is data
     char character = Serial.read(); // Receive a single character from the software serial port
        str.concat(character); // Add the received character to the receive buffer
  }
  if (str != ""){ // String is not empty
    Serial.println("BPM SET TO "+str); // Serial output
    bpm = str.toInt(); // Convert String to int
    str = "";  // Reset string to nothing
  }
  dLay = int(60000/bpm);  // BPM > MS
  digitalWrite(13, HIGH); // Set Red LED high
  digitalWrite(14, LOW);  //  Set Ylw LED low
  delay(dLay);            // wait for 60000 / BPM
  digitalWrite(13, LOW);  // set RED LED low
  digitalWrite(14, HIGH); // set Ylw LED High
  delay(dLay);            // wait for a second
}
