#include <FastLED.h>
#include <SoftwareSerial.h>
#include <CmdParser.hpp>
#include <AverageValue.h>
#include <EEPROM.h>
#include "constants.h"
#include "modes.h"
#include "hues.h"
#include "Animations.h"

// animation manager
Animations *animations;

// Bluetooth serial
SoftwareSerial btSerial(BLE_RXD, BLE_TXD);

// Command parser
CmdParser cmdParser;

// global brightness
uint8_t brightness = 127;

// turn off everything
boolean isStop = false;

// track if the EEPROM needs to be update
boolean needsSave = false;

// touch button value
AverageValue<long> buttonValue(40);
boolean buttonStop = false;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  brightness = EEPROM.read(BRIGHT_ADDR);
  uint8_t mode = EEPROM.read(MODE_ADDR);
  uint8_t hue = EEPROM.read(HUE_ADDR);
  uint8_t speed = EEPROM.read(SPEED_ADDR);
  
  animations = new Animations();
  animations->setHue(Hues(hue % NUM_HUES));
  animations->setMode(Modes(mode % NUM_MODES));
  animations->setSpeed(speed / 100.0);

  Serial.begin(9600);
  btSerial.begin(9600);

  FastLED.addLeds<LED_TYPE, PIN_LED, COLOR_ORDER>(animations->leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_MILLI_AMPS);
}

void loop() {
  processCommand(&Serial);
  processCommand(&btSerial);
  
  EVERY_N_MILLIS_I(timer, 10) {
    buttonValue.push(analogRead(PIN_BUTTON));
    boolean newButtonStop = buttonValue.average() <= THRES_BUTTON;
    if (newButtonStop != buttonStop) {
      isStop = buttonStop = newButtonStop;
    }
    
    timer.setPeriod(animations->period);
    animations->run(isStop);
    
    FastLED.setBrightness(brightness);
    FastLED.show();
    
    digitalWrite(LED_BUILTIN, isStop ? LOW : HIGH);
  }

  EVERY_N_SECONDS(5) {
    if (needsSave) {
      EEPROM.update(BRIGHT_ADDR, brightness);
      EEPROM.update(MODE_ADDR, animations->mode);
      EEPROM.update(HUE_ADDR, animations->hue);
      EEPROM.update(SPEED_ADDR, animations->speed * 100);
      needsSave = false;
    }
  }
}

void processCommand(Stream* serial) {
  if (serial->available() > 0) {
    CmdBuffer<32> cmdBuffer;

    if (cmdBuffer.readFromSerial(serial, 2000)) {
      if (cmdParser.parseCmd(cmdBuffer.getBuffer(), cmdBuffer.getBufferSize()) != CMDPARSER_ERROR) {
        if (cmdParser.equalCommand_P(PSTR("ON"))) {
          isStop = false;
          printOn(serial);
        }
        else if (cmdParser.equalCommand_P(PSTR("OFF"))) {
          isStop = true;
          printOn(serial);
        }
        else if (cmdParser.equalCommand_P(PSTR("BRIGHT"))) {
          brightness = atoi(cmdParser.getCmdParam(1));
          needsSave = true;
          printBright(serial);
        }
        else if (cmdParser.equalCommand_P(PSTR("BRIGHT+"))) {
          brightness = qadd8(brightness, BRIGHTNESS_STEP);
          needsSave = true;
          printBright(serial);
        }
        else if (cmdParser.equalCommand_P(PSTR("BRIGHT-"))) {
          brightness = qsub8(brightness, BRIGHTNESS_STEP);
          needsSave = true;
          printBright(serial);
        }
        else if (cmdParser.equalCommand_P(PSTR("MODE"))) {
          animations->setMode(getMode(cmdParser.getCmdParam(1)));
          needsSave = true;
          printMode(serial);
        }
        else if (cmdParser.equalCommand_P(PSTR("MODE+"))) {
          animations->setMode(Modes((animations->mode + 1) % NUM_MODES));
          needsSave = true;
          printMode(serial);
        }
        else if (cmdParser.equalCommand_P(PSTR("MODE-"))) {
          animations->setMode(Modes(animations->mode == 0 ? NUM_MODES - 1 : animations->mode - 1));
          needsSave = true;
          printMode(serial);
        }
        else if (cmdParser.equalCommand_P(PSTR("HUE"))) {
          animations->setHue(getHue(cmdParser.getCmdParam(1)));
          needsSave = true;
          printHue(serial);
        }
        else if (cmdParser.equalCommand_P(PSTR("HUE+"))) {
          animations->setHue(Hues((animations->hue + 1) % NUM_HUES));
          needsSave = true;
          printHue(serial);
        }
        else if (cmdParser.equalCommand_P(PSTR("HUE-"))) {
          animations->setHue(Hues(animations->hue == 0 ? NUM_HUES - 1 : animations->hue - 1));
          needsSave = true;
          printHue(serial);
        }
        else if (cmdParser.equalCommand_P(PSTR("SPEED"))) {
          animations->setSpeed(atof(cmdParser.getCmdParam(1)));
          needsSave = true;
          printSpeed(serial);
        }
        else if (cmdParser.equalCommand_P(PSTR("SPEED+"))) {
          animations->setSpeed(animations->speed + SPEED_STEP);
          needsSave = true;
          printSpeed(serial);
        }
        else if (cmdParser.equalCommand_P(PSTR("SPEED+"))) {
          animations->setSpeed(animations->speed - SPEED_STEP);
          needsSave = true;
          printSpeed(serial);
        }
        else if (cmdParser.equalCommand_P(PSTR("STATE"))) {
          printOn(serial);
          printBright(serial);
          printMode(serial);
          printHue(serial);
          printSpeed(serial);
        }
        else {
          serial->println(F("UNKNOWN COMMAND"));
        }
      } else {
        serial->println(F("COMMAND ERROR"));
      }
    } else {
      serial->println(F("READ ERROR"));
    }
  }
}

void printOn(Stream* serial) {
  serial->print(F("ON="));
  serial->println(isStop ? F("0") : F("1"));
}

void printBright(Stream* serial) {
  serial->print(F("BRIGHT="));
  serial->println(brightness);
}

void printMode(Stream* serial) {
  serial->print(F("MODE="));
  serial->println(getModeStr(animations->mode));
}

void printHue(Stream* serial) {
  serial->print(F("HUE="));
  serial->println(getHueStr(animations->hue));
}

void printSpeed(Stream* serial) {
  serial->print(F("SPEED="));
  serial->println(animations->speed);
}
