#include <FastLED.h>
#include <SoftwareSerial.h>
#include <CmdParser.hpp>
#include <AverageValue.h>
#include "constants.h"
#include "modes.h"
#include "hues.h"
#include "Animations.h"

// animation manager
Animations *animations;

// Bluetooth serial
SoftwareSerial serialControl(BLE_RXD, BLE_TXD);

// Command parser
CmdParser cmdParser;

// global brightness
uint8_t brightness = INITIAL_BRIGHTNESS;

// turn off everything
boolean isStop = false;

// touch button value
AverageValue<long> buttonValue(40);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  
  animations = new Animations();
  animations->setHue(H_RAINBOW);
  animations->setMode(M_RAINBOW);

  Serial.begin(9600);
  serialControl.begin(9600);

  FastLED.addLeds<LED_TYPE, PIN_LED, COLOR_ORDER>(animations->leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_MILLI_AMPS);
}

void loop() {
  while (serialControl.available() > 0) {
    CmdBuffer<32> cmdBuffer;

    if (cmdBuffer.readFromSerial(&serialControl, 2000)) {
      if (cmdParser.parseCmd(cmdBuffer.getBuffer(), cmdBuffer.getBufferSize()) != CMDPARSER_ERROR) {
        if (cmdParser.equalCommand_P(PSTR("BRIGHT"))) {
          brightness = atoi(cmdParser.getCmdParam(1));
          serialControl.print(F("BRIGHT="));
          serialControl.println(brightness);
        }
        else if (cmdParser.equalCommand_P(PSTR("BRIGHT+"))) {
          brightness = qadd8(brightness, BRIGHTNESS_STEP);
          serialControl.print(F("BRIGHT="));
          serialControl.println(brightness);
        }
        else if (cmdParser.equalCommand_P(PSTR("BRIGHT-"))) {
          brightness = qsub8(brightness, BRIGHTNESS_STEP);
          serialControl.print(F("BRIGHT="));
          serialControl.println(brightness);
        }
        else if (cmdParser.equalCommand_P(PSTR("MODE"))) {
          animations->setMode(getMode(cmdParser.getCmdParam(1)));
          serialControl.print(F("MODE="));
          serialControl.println(getModeStr(animations->mode));
        }
        else if (cmdParser.equalCommand_P(PSTR("MODE+"))) {
          animations->setMode(Modes((animations->mode + 1) % NUM_MODES));
          serialControl.print(F("MODE="));
          serialControl.println(getModeStr(animations->mode));
        }
        else if (cmdParser.equalCommand_P(PSTR("MODE-"))) {
          animations->setMode(Modes(animations->mode == 0 ? NUM_MODES - 1 : animations->mode - 1));
          serialControl.print(F("MODE="));
          serialControl.println(getModeStr(animations->mode));
        }
        else if (cmdParser.equalCommand_P(PSTR("HUE"))) {
          animations->setHue(getHue(cmdParser.getCmdParam(1)));
          serialControl.print(F("HUE="));
          serialControl.println(getHueStr(animations->hue));
        }
        else if (cmdParser.equalCommand_P(PSTR("HUE+"))) {
          animations->setHue(Hues((animations->hue + 1) % NUM_HUES));
          serialControl.print(F("HUE="));
          serialControl.println(getHueStr(animations->hue));
        }
        else if (cmdParser.equalCommand_P(PSTR("HUE-"))) {
          animations->setHue(Hues(animations->hue == 0 ? NUM_HUES - 1 : animations->hue - 1));
          serialControl.print(F("HUE="));
          serialControl.println(getHueStr(animations->hue));
        }
        else {
          serialControl.println(F("UNKNOWN COMMAND"));
        }
      } else {
        serialControl.println(F("COMMAND ERROR"));
      }
    } else {
      serialControl.println(F("READ ERROR"));
    }
  }
  
  EVERY_N_MILLIS_I(timer, 10) {
    buttonValue.push(analogRead(PIN_BUTTON));
    isStop = buttonValue.average() <= THRES_BUTTON;
    
    timer.setPeriod(animations->period);
    animations->run(isStop);
    
    FastLED.setBrightness(brightness);
    FastLED.show();
    
    digitalWrite(LED_BUILTIN, isStop ? LOW : HIGH);
  }
}
