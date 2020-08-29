#pragma once

#include <FastLED.h>
#include "hues.h"

class ColorGenerator {
  public:
    enum Hues hue;
    int counter;

    void setHue(enum Hues newHue) {
      hue = newHue;
      counter = 0;
    }

    CRGB next(int inc) {
      counter+= inc;

      if (counter >= 255) {
        counter-= 255;
      }
      
      return fromPalette(counter);
    }

    CRGB random() {
      return fromPalette(random8());
    }

    CRGB fromPalette(int value) {
      return ColorFromPalette(palette(), value, 255, LINEARBLEND);
    }

    // hue ranges from randomColor library
    CRGBPalette16 palette() {
      switch (hue) {
        case H_RAINBOW:
           return RainbowColors_p;
        case H_WHITE:
           return WhitePalette;
        case H_RED:
           return RedPalette;
        case H_ORANGE:
           return OrangePalette;
        case H_YELLOW:
           return YellowPalette;
        case H_GREEN:
           return GreenPalette;
        case H_BLUE:
           return BluePalette;
        case H_PURPLE:
           return PurplePalette;
        case H_PINK:
           return PinkPalette;
        default:
           return BlackPalette;
      }
    }
};
