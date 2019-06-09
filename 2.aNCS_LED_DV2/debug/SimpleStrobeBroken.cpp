void simpleStrobe () {
  int myDelay = map(variableDelay, 0, 255, (standardDelay / 8), (standardDelay * 8));
  fill_solid( LEDStart, NoLEDs, CRGB::Black);
  const uint8_t kStrobeCycleLength = 6; // light every Nth frame
  static uint8_t sStrobePhase = 0;
  sStrobePhase = sStrobePhase + 1;
  colorIndex++;
  if (colorIndex > 256) {
    colorIndex = 1;
  }
  if ( sStrobePhase >= kStrobeCycleLength ) {
    sStrobePhase = 0;
  }
  if ( sStrobePhase == 0 ) {
    uint8_t dashperiod = beatsin8( 8, 4, 10);//First argument = cycles per minute
    uint8_t dashwidth = (dashperiod / 4) + 1;
    uint8_t zoomBPM = STROBE_BEATS_PER_MINUTE;
    int8_t  dashmotionspeed = beatsin8( (zoomBPM / 2), 1, dashperiod);
    if (dashmotionspeed >= (dashperiod / 2)) {
      dashmotionspeed = 0 - (dashperiod - dashmotionspeed );
    }
    uint8_t cycle = beat8(2); // two cycles per minute
    uint8_t easedcycle = ease8InOutCubic( ease8InOutCubic( cycle));
    uint8_t wavecycle = cubicwave8( easedcycle);
    // uint8_t hueShift = 0; // NO SHIFT OF HUE IN COLOUR (we should rebuild in RGB...)
    uint8_t hueShift = scale8( wavecycle, 130); // METHOD HOW HUE VALUE SHIFTS
    uint8_t strobesPerPosition = 2; // try 1..4
    strobeCore(dashperiod, dashwidth, dashmotionspeed, strobesPerPosition, hueShift);
  }
  if (myDelay == 0){
    myDelay = 81;
  }
  if (individualStripMode == 1) {
    delayStore[stripNumber] = myDelay;
  }
  DPRINT("DELAY ");
  DPRINTLN(myDelay);
  DFLUSH();
  currentDelay = myDelay;
}
void strobeCore(uint8_t dashperiod, uint8_t dashwidth, int8_t  dashmotionspeed, uint8_t stroberepeats, uint8_t huedelta) {
  
  static uint8_t sRepeatCounter = 0;
  static int8_t sStartPosition = LEDStart;
  DPRINTLN("STROBE CORE ");
  DFLUSH();
  static uint8_t sStartHue = 0;
  sStartHue += 1; //Shift the Colour little by little
  sRepeatCounter = sRepeatCounter + 1;
  if ( sRepeatCounter >= stroberepeats) {
    DPRINTLN("sRepeatCounter >= stroberepeats");
    DFLUSH();
    sRepeatCounter = 0;
    sStartPosition = sStartPosition + dashmotionspeed;
    
    if ( sStartPosition >= dashperiod ) {
       DPRINTLN("sStartPosition >= dashperiod");
       DFLUSH();
      while ( sStartPosition >= dashperiod) {
        sStartPosition -= dashperiod;
      }
      sStartHue  -= huedelta;
    }
    else if ( sStartPosition < 0) {
      while ( sStartPosition < 0) {
        sStartPosition += dashperiod;
      }
      sStartHue  += huedelta;
    }
  }
  const uint8_t kSaturation = 208; // WHITE >> CURRENT COLOUR control (def 208)
  const uint8_t kValue = 200; // Brightness??
  strobeDraw( sStartPosition, LEDEnd, dashperiod, dashwidth, sStartHue, huedelta, kSaturation, kValue);
}
static void strobeDraw(uint8_t startpos, uint16_t lastpos, uint8_t period, uint8_t width, uint8_t huestart, uint8_t huedelta, uint8_t saturation, uint8_t value) {
  uint8_t hue = huestart;
  for ( uint16_t i = startpos; i <= lastpos; i += period) {
    CRGB color = CHSV( hue, saturation, value);
    //CRGB color = CRGB::Blue; // USE TO COMPLETELY BYPASS HSV Change Scheme
    uint16_t pos = i;
    for ( uint8_t w = 0; w < width; w++) {
      leds[pos] = ColorFromPalette(currentPalette, colorIndex);
      pos++;
      if (pos >= LEDEnd) {
        break;
      }
    }
    hue += huedelta;
  }
}