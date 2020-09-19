#pragma once

#include <FastLED.h>

static CRGBPalette16 BlackPalette(CRGB::Black, CRGB::Black);
static CRGBPalette16 WhitePalette(CHSV(0, 0, 55), CHSV(0, 0, 255), CHSV(0, 0, 55));
static CRGBPalette16 RedPalette(CHSV(242, 255, 255), CHSV(13, 255, 255), CHSV(242, 255, 255));
static CRGBPalette16 YellowPalette(CHSV(13, 255, 255), CHSV(44, 255, 255), CHSV(13, 255, 255));
static CRGBPalette16 GreenPalette(CHSV(44, 255, 255), CHSV(126, 255, 255), CHSV(44, 255, 255));
static CRGBPalette16 BluePalette(CHSV(126, 255, 255), CHSV(200, 255, 255), CHSV(126, 255, 255));
static CRGBPalette16 PinkPalette(CHSV(200, 255, 255), CHSV(242, 255, 255), CHSV(200, 255, 255));

// TODO DEFINE_GRADIENT_PALETTE(name) { stops }

enum Colors {
  H_RAINBOW,
  H_RED,
  H_YELLOW,
  H_GREEN,
  H_BLUE,
  H_PINK,
  H_WHITE,
  NUM_COLORS
};

enum Colors getColor(char* str) {
  if (strcasecmp_P(str, PSTR("RAINBOW")) == 0) {
    return H_RAINBOW;
  }
  if (strcasecmp_P(str, PSTR("RED")) == 0) {
    return H_RED;
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
  if (strcasecmp_P(str, PSTR("PINK")) == 0) {
    return H_PINK;
  }
  if (strcasecmp_P(str, PSTR("WHITE")) == 0) {
    return H_WHITE;
  }
  return H_RAINBOW;
}

String getColorStr(enum Colors color) {
  switch (color) {
    case H_RAINBOW:
      return F("RAINBOW");
    case H_RED:
      return F("RED");
    case H_YELLOW:
      return F("YELLOW");
    case H_GREEN:
      return F("GREEN");
    case H_BLUE:
      return F("BLUE");
    case H_PINK:
      return F("PINK");
    case H_WHITE:
      return F("WHITE");
    default:
      return F("UNKNOWN");
  }
}

CRGBPalette16 getPalette(enum Colors color) {
  switch (color) {
    case H_RAINBOW:
       return RainbowColors_p;
    case H_WHITE:
       return WhitePalette;
    case H_RED:
       return RedPalette;
    case H_YELLOW:
       return YellowPalette;
    case H_GREEN:
       return GreenPalette;
    case H_BLUE:
       return BluePalette;
    case H_PINK:
       return PinkPalette;
    default:
       return BlackPalette;
  }
}
