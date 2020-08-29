#pragma once

#include <FastLED.h>

enum Hues {
  H_RAINBOW,
  H_RED,
  H_ORANGE,
  H_YELLOW,
  H_GREEN,
  H_BLUE,
  H_PURPLE,
  H_PINK,
  H_WHITE,
  NUM_HUES
};

enum Hues getHue(char* str) {
  if (strcasecmp_P(str, PSTR("RAINBOW")) == 0) {
    return H_RAINBOW;
  }
  if (strcasecmp_P(str, PSTR("ORANGE")) == 0) {
    return H_ORANGE;
  }
  if (strcasecmp_P(str, PSTR("YELLOW")) == 0) {
    return H_YELLOW;
  }
  if (strcasecmp_P(str, PSTR("GREEN")) == 0) {
    return H_GREEN;
  }
  if (strcasecmp_P(str, PSTR("BLUE")) == 0) {
    return H_BLUE;
  }
  if (strcasecmp_P(str, PSTR("PURPLE")) == 0) {
    return H_PURPLE;
  }
  if (strcasecmp_P(str, PSTR("PINK")) == 0) {
    return H_PINK;
  }
  if (strcasecmp_P(str, PSTR("WHITE")) == 0) {
    return H_WHITE;
  }
  return H_RAINBOW;
}

// hue ranges from randomColor library
static CRGBPalette16 BlackPalette(CRGB::Black, CRGB::Black);
static CRGBPalette16 WhitePalette(CHSV(0, 0, 125), CHSV(0, 055, 255), CHSV(0, 0, 125));
static CRGBPalette16 RedPalette(CHSV(237, 255, 255), CHSV(13, 255, 255), CHSV(237, 255, 255));
static CRGBPalette16 OrangePalette(CHSV(13, 255, 255), CHSV(33, 255, 255), CHSV(13, 255, 255));
static CRGBPalette16 YellowPalette(CHSV(33, 255, 255), CHSV(44, 255, 255), CHSV(33, 255, 255));
static CRGBPalette16 GreenPalette(CHSV(44, 255, 255), CHSV(126, 255, 255), CHSV(44, 255, 255));
static CRGBPalette16 BluePalette(CHSV(126, 255, 255), CHSV(182, 255, 255), CHSV(126, 255, 255));
static CRGBPalette16 PurplePalette(CHSV(182, 255, 255), CHSV(200, 255, 255), CHSV(182, 255, 255));
static CRGBPalette16 PinkPalette(CHSV(200, 255, 255), CHSV(200, 237, 255), CHSV(200, 255, 255));
