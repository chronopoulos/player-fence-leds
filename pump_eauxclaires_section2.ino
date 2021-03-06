/*
Copyright (c) 2018 Chris Chronopoulos

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <FastLED.h>

#define DATA_PIN 2
#define NCHIMES 15
#define LEDS_PER_CHIME 5

#define HUE_C 42 // yellow
#define HUE_D 74 // light green
#define HUE_E 14 // orange
#define HUE_F 170 // blue
#define HUE_G 87 // dark green
#define HUE_A 215 // magenta
#define HUE_B 103 // puke green
#define HUE_Bb 188 // deep purple

CRGB leds[NCHIMES * LEDS_PER_CHIME];

float intensity[NCHIMES];

bool autoMode;
float phase_autoMode[NCHIMES]; // assigned randomly in setup()
float autoPeriod_ms = 6000;

uint8_t hues[15] = {
  HUE_D,
  HUE_E,
  HUE_C,
  HUE_G,
  HUE_A,
  HUE_G,
  HUE_D,
  HUE_E,
  HUE_B,
  HUE_A,
  HUE_G,
  HUE_E,
  HUE_A,
  HUE_C,
  HUE_E
};

float decayRate = 0.02; // higher value = faster decay

unsigned long timestamp_ms = 0;
unsigned long timeout_ms = 30000;

int tmpChime;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void setup() {

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NCHIMES*LEDS_PER_CHIME);

  for (int i=0; i<NCHIMES; i++) {
    intensity[i] = 0.;
    phase_autoMode[i] = random(autoPeriod_ms);
  }

  Serial.begin(-1); // full USB speed is used

}

void loop() {

  // read serial messages, and update intensity[] accordingly
  while (Serial.available() > 0) {

    tmpChime = Serial.read();
    if (tmpChime >= 0) {
      intensity[tmpChime] = 1.0;
      timestamp_ms = millis();
    }

  }

  autoMode = ((millis() - timestamp_ms) > timeout_ms);

  if (autoMode) {
    
    evolve_autoMode();
    
  } else {
    
    evolve_triggerMode();
    
  }

  delay(15);

}

void evolve_autoMode(void) {

    // evolve the leds array
    for (int i=0; i<NCHIMES; i++) {
  
      for (int j=0; j<LEDS_PER_CHIME; j++) {
        
        leds[i*LEDS_PER_CHIME + j].setHSV(hues[i], 255,
                                          sq(sin((millis()-phase_autoMode[i])/autoPeriod_ms)) * 200 + 55);
                                          
      }
  
    }
    
    FastLED.show();

}

void evolve_triggerMode(void) {

    // evolve the leds array
    for (int i=0; i<NCHIMES; i++) {
  
      for (int j=0; j<LEDS_PER_CHIME; j++) {
        leds[i*LEDS_PER_CHIME + j].setHSV(hues[i], 255, intensity[i]*255);
      }
  
      intensity[i] /= (1 + decayRate);
  
    }
    
    FastLED.show();

}
