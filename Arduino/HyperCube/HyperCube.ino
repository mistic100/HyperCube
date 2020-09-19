#include <FastLED.h>
#include <SoftwareSerial.h>
#include <CmdParser.hpp>
#include <AverageValue.h>
#include <EEPROM.h>
#include "constants.h"
#include "modes.h"
#include "colors.h"
#include "custom.h"
#include "Animations.h"

// animation manager
Animations *animations;

// Bluetooth serial
SoftwareSerial btSerial(BLE_RXD, BLE_TXD);

// Command parser
CmdParser cmdParser;

// turn off everything
boolean isStop = false;

// track if the EEPROM needs to be update
boolean needsSave = false;

// touch button value
AverageValue<long> buttonValue(40);
boolean buttonStop = false;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  
  animations = new Animations();

  uint8_t brightness = EEPROM.read(BRIGHT_ADDR);
  if (brightness == 0) {
    brightness = BRIGHTNESS_BASE;
  }
  uint8_t mode = EEPROM.read(MODE_ADDR);
  uint8_t color = EEPROM.read(COLOR_ADDR);
  uint8_t speed = EEPROM.read(SPEED_ADDR);
  if (speed == 0) {
    speed = SPEED_BASE;
  }

  if (EEPROM.read(PATTERN_ADDR) == CUSTOM_PATTERN_VERSION) {
    EEPROM.get(PATTERN_ADDR, animations->pattern);
  }

  animations->setBrightness(brightness);
  animations->setColor(Colors(color % NUM_COLORS));
  animations->setSpeed(speed);
  animations->setMode(Modes(mode == M_PATTERN ? mode : mode % NUM_MODES));

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
    
    FastLED.setBrightness(animations->brightness);
    FastLED.show();
    
    digitalWrite(LED_BUILTIN, isStop ? LOW : HIGH);
  }

  EVERY_N_SECONDS(5) {
    if (needsSave) {
      EEPROM.update(BRIGHT_ADDR, animations->brightness);
      EEPROM.update(MODE_ADDR, animations->mode);
      EEPROM.update(COLOR_ADDR, animations->color);
      EEPROM.update(SPEED_ADDR, animations->speed);
      EEPROM.put(PATTERN_ADDR, animations->pattern);
      needsSave = false;
    }
  }
}

void processCommand(Stream* serial) {
  if (serial->available() > 0) {
    static CmdBuffer<CMD_BUFFER_SIZE> cmdBuffer;
    cmdParser.setOptKeyValue(true);

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
          animations->setBrightness(atoi(cmdParser.getCmdParam(1)));
          needsSave = true;
          printBright(serial);
        }
        else if (cmdParser.equalCommand_P(PSTR("BRIGHT+"))) {
         animations->setBrightness(qadd8(animations->brightness, BRIGHTNESS_STEP));
          needsSave = true;
          printBright(serial);
        }
        else if (cmdParser.equalCommand_P(PSTR("BRIGHT-"))) {
          animations->setBrightness(qsub8(animations->brightness, BRIGHTNESS_STEP));
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
        else if (cmdParser.equalCommand_P(PSTR("COLOR"))) {
          animations->setColor(getColor(cmdParser.getCmdParam(1)));
          needsSave = true;
          printColor(serial);
        }
        else if (cmdParser.equalCommand_P(PSTR("COLOR+"))) {
          animations->setColor(Colors((animations->color + 1) % NUM_COLORS));
          needsSave = true;
          printColor(serial);
        }
        else if (cmdParser.equalCommand_P(PSTR("COLOR-"))) {
          animations->setColor(Colors(animations->color == 0 ? NUM_COLORS - 1 : animations->color - 1));
          needsSave = true;
          printColor(serial);
        }
        else if (cmdParser.equalCommand_P(PSTR("SPEED"))) {
          animations->setSpeed(atof(cmdParser.getCmdParam(1)));
          needsSave = true;
          printSpeed(serial);
        }
        else if (cmdParser.equalCommand_P(PSTR("SPEED+"))) {
          animations->setSpeed(qadd8(animations->speed, SPEED_STEP));
          needsSave = true;
          printSpeed(serial);
        }
        else if (cmdParser.equalCommand_P(PSTR("SPEED+"))) {
          animations->setSpeed(qsub8(animations->speed, SPEED_STEP));
          needsSave = true;
          printSpeed(serial);
        }
        else if (cmdParser.equalCommand_P(PSTR("PATTERN"))) {
          if (parseCustomPattern(&(animations->pattern),
              cmdParser.getValueFromKey_P(PSTR("C")),
              cmdParser.getValueFromKey_P(PSTR("A")),
              cmdParser.getValueFromKey_P(PSTR("Z"))) != CUSTOM_PATTERN_ERROR) {
            animations->setMode(M_PATTERN);
            needsSave = true;
            printPattern(serial);
          }
          else {
            serial->println(F("PATTERN_ERROR"));
          }
        }
        else if (cmdParser.equalCommand_P(PSTR("STATE"))) {
          printOn(serial);
          printBright(serial);
          printMode(serial);
          printColor(serial);
          printSpeed(serial);
          printPattern(serial);
        }
        else {
          serial->println(F("UNKNOWN_COMMAND"));
        }
      } else {
        serial->println(F("COMMAND_ERROR"));
      }
    } else {
      serial->println(F("READ_ERROR"));
    }
  }
}

void printOn(Stream* serial) {
  serial->print(F("ON"));
  serial->print(F(" "));
  serial->println(isStop ? F("0") : F("1"));
}

void printBright(Stream* serial) {
  serial->print(F("BRIGHT"));
  serial->print(F(" "));
  serial->println(animations->brightness);
}

void printMode(Stream* serial) {
  serial->print(F("MODE"));
  serial->print(F(" "));
  serial->println(getModeStr(animations->mode));
}

void printColor(Stream* serial) {
  serial->print(F("COLOR"));
  serial->print(F(" "));
  serial->println(getColorStr(animations->color));
}

void printSpeed(Stream* serial) {
  serial->print(F("SPEED"));
  serial->print(F(" "));
  serial->println(animations->speed);
}

void printPattern(Stream* serial) {
  serial->print(F("PATTERN"));
  serial->print(F(" "));
  serial->println(getPatternStr(&animations->pattern));
}
