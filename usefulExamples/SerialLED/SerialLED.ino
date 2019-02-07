/* 
 *  This sketch allows user to type "on" or "1" to turn the built in led on 
 *  And "off" or "0" to turn the LED off. Any other input is ignored. 
 *  Press Ctrl+Shift+M, the magnifiying glass button by close, 
 *  or on the top menu Tools>Serial Monitor to show it. 
 *  Enter commands in the top line.
 */
// hi 

#define LED_PIN 13  // Sets LED_PIN to pin 13 (on board LED)
// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(LED_PIN, OUTPUT); // Change the mode of LED_PIN (13) to OUTPUT
  Serial.begin(115200); // Starts Serial at 115200 baud rate
  Serial.println("Ready for input"); // Prints a line to the console window
}; // setup ends

// the loop function runs over and over again forever
void loop() {
  
  // Here we make all the decisions, firstly if there is Serial.available()
  // this will return true if something was entered in the console and 
  // false if not so we don't need to do: if(Serial.available() == true)
  if(Serial.available()){ 
    
    // Create a new String type variable, in this case called stringToCompare, 
    // It needs to be set to "" so we can add to it, Then let the user know 
    // we got their message
    String stringToCompare = "";  
    Serial.print("got Serial "); 
    
    // So, we determined there is Serial data, We want to read all of it, 
    // The next part accomplishes this: while(Serial.available()) will repeat 
    // provided that its Serial.available() is still true. 
    while(Serial.available()){
       
      // Here we create a variable, incomingChar, it holds a single character
      // it is currently empty
      // because incomingChar is a char type, anything we save to it will be
      // converted to char type, which is a single alphanumeric character
      char incomingChar;
      
      // We read one byte from Serial, and save it as incomingChar
      incomingChar = Serial.read(); 
      // now, We add that single character (concatenate) to the end
      // of stringToCompare 
      stringToCompare.concat(incomingChar);
      // and repeat until Serial.available() is false
    } // while(Serial.available()) ends Serial.available() is false

    // Convert stringToCompare to all upper case so on, On, ON and off, Off, OFF etc all work.
    stringToCompare.toUpperCase(); 

    // Removes all empty spaces from stringToCompare
    stringToCompare.trim();
    
    // Comparison time... The Serial.println(...) lines let the user know the 
    // outcome of their console input
    if(stringToCompare == ""){ // stringToCompare is still empty
      Serial.println("Was Nothing, Not sure how");
      // Do nothing 
    } // if(stringToCompare == "") ends 

    // This statement matches "ON" or "1", either work. 
    else if( ( stringToCompare == "ON" ) or ( stringToCompare == "1" ) ){
      Serial.print("stringToCompare was");
      Serial.print(stringToCompare); // Print the variable
      Serial.println(" Turning LED on!");
      digitalWrite(LED_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    } // else if( ( stringToCompare == "ON" ) or ( stringToCompare == "1" ) ) ends

    // Here is the statement for off, Again either "OFF" or "0" work
    else if( ( stringToCompare == "OFF" ) or ( stringToCompare == "0" ) ){
      Serial.print("stringToCompare was "); // Print the start
      Serial.print(stringToCompare); // Print the variable
      Serial.println(" , Turning LED off!"); // print the end, and end the line
      digitalWrite(LED_PIN, LOW);    // turn the LED off by making the voltage LOW
    } // else if( ( stringToCompare == "OFF" ) or ( stringToCompare == "0" ) ) ends

    // Getting here means that we don't have an empty string, we definately have something
    // but what we have is not "ON", "OFF", "1", or "0"
    else {
      Serial.println("Wrong input detected. Nothing happens");
      // nothing happens here
    } // else ends
  } // if(Serial.available()) ends
  
  // wait 1 second for serial input
  delay(1000);// wait for a second before updating again. 
}; // loop ends (well it just starts again but it finishes)
