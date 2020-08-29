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
uint8_t brightness = INITIAL_BRIGHTNESS;

// turn off everything
boolean isStop = false;

void setup() {
  animations = new Animations();
  animations->setHue(H_RAINBOW);
  animations->setMode(M_RANDOM);

  Serial.begin(9600);
  serialControl.begin(9600);

  FastLED.addLeds<LED_TYPE, PIN_LED, COLOR_ORDER>(animations->leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_MILLI_AMPS);
}

void loop() {
  isStop = analogRead(PIN_BUTTON) == 0;
  
  while (serialControl.available() > 0) {
    CmdBuffer<32> cmdBuffer;
    
    if (cmdBuffer.readFromSerial(&serialControl, 2000) && cmdParser.parseCmd(&cmdBuffer) != CMDPARSER_ERROR) {
      if (cmdParser.equalCommand_P(PSTR("BRIGHT"))) {
        brightness = atoi(cmdParser.getCmdParam(1));
      }
      else if (cmdParser.equalCommand_P(PSTR("BRIGHT+"))) {
        brightness = qadd8(brightness, BRIGHTNESS_STEP);
      }
      else if (cmdParser.equalCommand_P(PSTR("BRIGHT-"))) {
        brightness = qsub8(brightness, BRIGHTNESS_STEP);
      }
      else if (cmdParser.equalCommand_P(PSTR("MODE"))) {
        animations->setMode(getMode(cmdParser.getCmdParam(1)));
      }
      else if (cmdParser.equalCommand_P(PSTR("MODE+"))) {
        animations->setMode(Modes((animations->mode + 1) % NUM_MODES));
      }
      else if (cmdParser.equalCommand_P(PSTR("MODE-"))) {
        animations->setMode(Modes(animations->mode == 0 ? NUM_MODES - 1 : animations->mode - 1));
      }
      else if (cmdParser.equalCommand_P(PSTR("HUE"))) {
        animations->setHue(getHue(cmdParser.getCmdParam(1)));
      }
      else if (cmdParser.equalCommand_P(PSTR("HUE+"))) {
        animations->setHue(Hues((animations->hue + 1) % NUM_HUES));
      }
      else if (cmdParser.equalCommand_P(PSTR("HUE-"))) {
        animations->setHue(Hues(animations->hue == 0 ? NUM_HUES - 1 : animations->hue - 1));
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
