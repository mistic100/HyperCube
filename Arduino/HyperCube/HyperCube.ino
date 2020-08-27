#include <FastLED.h>
#include <SoftwareSerial.h>
#include <CmdParser.hpp>

#include "constants.h"
#include "modes.h"
#include "hues.h"
#include "Animations.h"

// animation manager
Animations *animations;

// Bluetooth serial
SoftwareSerial serialControl(BLE_TXD, BLE_RXD);

// Command parser
CmdParser cmdParser;

// global brightness
uint8_t brightness = 100;

// turn off everything
boolean isStop = false;

boolean buttonValue = false;

void setup() {
  animations = new Animations();
  animations->setHue(H_RAINBOW);
  animations->setCurrentMode(M_RAINBOW);

  Serial.begin(9600); 
  serialControl.begin(9600);

  FastLED.addLeds<LED_TYPE, PIN_LED, COLOR_ORDER>(animations->leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_MILLI_AMPS);
}

void loop() {
  boolean val = analogRead(PIN_BUTTON) > 0;
  if (val != buttonValue) {
    isStop = !val;
    buttonValue = val;
  }
  
  while (serialControl.available() > 0) {
    CmdBuffer<32> cmdBuffer;
    
    if (cmdBuffer.readFromSerial(&serialControl, 2000) && cmdParser.parseCmd(&cmdBuffer) != CMDPARSER_ERROR) {
      if (cmdParser.equalCommand_P(PSTR("ON"))) {
        isStop = false;
      }
      else if (cmdParser.equalCommand_P(PSTR("OFF"))) {
        isStop = true;
      }
      else if (cmdParser.equalCommand_P(PSTR("BRIGHT"))) {
        brightness = atoi(cmdParser.getCmdParam(1));
      }
      else if (cmdParser.equalCommand_P(PSTR("MODE"))) {
        uint8_t mode = atoi(cmdParser.getCmdParam(1));
        animations->setCurrentMode(Modes(mode % NUM_MODES));
      }
      else if (cmdParser.equalCommand_P(PSTR("COLOR"))) {
        uint8_t r = atoi(cmdParser.getCmdParam(1));
        uint8_t g = atoi(cmdParser.getCmdParam(2));
        uint8_t b = atoi(cmdParser.getCmdParam(3));
        animations->setCurrentMode(M_STATIC);
        animations->staticColor = CRGB(r, g, b);
      }
    }
  }
  
  EVERY_N_MILLIS_I(timer, 10) {
    timer.setPeriod(animations->period);
    animations->run(isStop);
    FastLED.setBrightness(brightness);
    FastLED.show();
  }
}
