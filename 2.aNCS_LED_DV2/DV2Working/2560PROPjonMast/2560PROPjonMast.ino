/**
 * @todo Make DV2 Pjon master receive i2c clock pulses
 * @body Master should broadcast BPM if it ever changes and broadcast a pulse on 4, 16, 32, 64 clock messages received. 
 */
#include <PJON.h>
#include <SoftwareSerial.h>
#include <FastLED.h>
