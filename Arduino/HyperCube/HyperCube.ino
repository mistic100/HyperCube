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
CmdBuffer<CMD_BUFFER_SIZE> cmdBuffer;

// pause LED processing
boolean pause = false;

// track if the EEPROM needs to be update
boolean needsSave = false;

// touch button value
AverageValue<long> buttonValue(40);
boolean buttonOff = false;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  
  animations = new Animations();

  uint8_t brightness = EEPROM.read(ADDR_BRIGHT);
  if (brightness == 0) {
    brightness = BRIGHTNESS_BASE;
  }
  uint8_t mode = EEPROM.read(ADDR_MODE);
  uint8_t color = EEPROM.read(ADDR_COLOR);
  uint8_t speed = EEPROM.read(ADDR_SPEED);
  if (speed == 0) {
    speed = SPEED_BASE;
  }

  if (EEPROM.read(ADDR_PATTERN) == CUSTOM_PATTERN_VERSION) {
    EEPROM.get(ADDR_PATTERN, animations->pattern);
  }

  animations->setBrightness(brightness);
  animations->setColor(Colors(color % NUM_COLORS));
  animations->setSpeed(speed);
  animations->setMode(Modes(mode == M_PATTERN ? mode : mode % NUM_MODES));

  Serial.begin(9600);
  btSerial.begin(BLE_BAUD);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(animations->leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_MILLI_AMPS);
}

void loop() {
  if (!pause) {
    if (btSerial.available() && btSerial.read() == BLE_CTRL_CHAR) {
      // force empty the serial buffer
      delay(100);
      while (btSerial.available()) btSerial.read();
      
      pause = true;
      cmdBuffer.clear();
      btSerial.println(F("OK"));
    } else {
      processCommand(&Serial);
    }
  } else {
    if (processCommand(&btSerial)) {
      pause = false;
    }
  }
  
  EVERY_N_MILLIS_I(timer, 10) {
    buttonValue.push(analogRead(BUTTON_PIN));
    boolean newButtonOff = buttonValue.average() <= BUTTON_THRES;
    if (newButtonOff != buttonOff) {
      buttonOff = newButtonOff;
      if (buttonOff) {
        animations->off();
      } else {
        animations->on();
      }
    }

    if (!pause) {
      timer.setPeriod(animations->period);
      animations->run();
    
      FastLED.setBrightness(animations->brightness);
      FastLED.show();
    
      digitalWrite(LED_BUILTIN, animations->isOff ? LOW : HIGH);
    }
  }

  EVERY_N_SECONDS(5) {
    if (!pause) {
      printOn(&btSerial);
    }
    
    if (needsSave) {
      EEPROM.update(ADDR_BRIGHT, animations->brightness);
      EEPROM.update(ADDR_MODE, animations->mode);
      EEPROM.update(ADDR_COLOR, animations->color);
      EEPROM.update(ADDR_SPEED, animations->speed);
      EEPROM.put(ADDR_PATTERN, animations->pattern);
      needsSave = false;
    }
  }
}

boolean processCommand(Stream* serial) {
  // don't use CmdBuffer#readFromSerial which expects to receive the whole command at once
  bool done = false;
  while (serial->available()) {
      if (cmdBuffer.readSerialChar(serial)) {
          done = true;
          break;
      }
  }

  if (done) {
    cmdParser.setOptKeyValue(true);
    
    if (cmdParser.parseCmd(cmdBuffer.getBuffer(), cmdBuffer.getBufferSize()) != CMDPARSER_ERROR) {
      if (cmdParser.equalCommand_P(PSTR("ON"))) {
        animations->on();
        printOn(serial);
      }
      else if (cmdParser.equalCommand_P(PSTR("OFF"))) {
        animations->off();
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
        serial->print(F("UNKNOWN_COMMAND "));
        serial->println((char*) cmdBuffer.getBuffer());
      }
    } else {
      serial->println(F("COMMAND_ERROR"));
    }
  }

  return done;
}

void printOn(Stream* serial) {
  serial->print(F("ON"));
  serial->print(F(" "));
  serial->println(animations->isOff ? F("0") : F("1"));
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
