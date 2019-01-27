Purpose.
To limit memory usage of debug commands (ie. Serial.print ect.) and speed up program runtime.
See https://forum.arduino.cc/index.php?topic=215334.0&fbclid=IwAR1Gbxq1rMmKnOw0rwddKf3DgMZeqCU4U4JwrDk2q0UA8MdvJbHo6zpgyAo
comment 8 for more information.
Comment out line 1 (#define DEBUG) to turn debug mode off.
To add to your code copy line 1...8 to the top of your file.
Use DPRINT instead of Serial.print and DPRINTLN instead of Serial.println.
Other functions can be added using the same method.  See above link for further details.
