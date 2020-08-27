#pragma once

#include <FastLED.h>

enum Hues {
  H_RAINBOW,
  H_WHITE,
  H_RED,
  H_ORANGE,
  H_YELLOW,
  H_GREEN,
  H_BLUE,
  H_PURPLE,
  H_PINK
};

class ColorGenerator {
  public:
    enum Hues hue;
    int counter;
    int dir;

    void setHue(enum Hues newHue) {
      hue = newHue;
      reset();
    }

    void reset() {
      counter = 0;
      dir = 1;
    }

    CHSV next(int inc) {
      counter += inc * dir;

      if (counter > 255) {
        counter = 255;
        dir = -dir;
      }
      if (counter < 0) {
        counter = 0;
        dir = -dir;
      }
      
      return getFromRange(counter);
    }

    CHSV random() {
      return getFromRange(random8());
    }

  private:
    // hue ranges from randomColor library
    CHSV getFromRange(int value) {
      if (hue == H_WHITE) {
        int v = map(value, 0, 255, 125, 255);
        return CHSV(0, 0, v);
      }

      int h;
      switch (hue) {
          case H_RAINBOW:
             h = value;
             break;
          case H_RED:
             h =  map(value, 0, 255, -18, 13);
             break;
          case H_ORANGE:
             h = map(value, 0, 255, 13, 33);
             break;
          case H_YELLOW:
             h = map(value, 0, 255, 33, 44);
             break;
          case H_GREEN:
             h = map(value, 0, 255, 44, 126);
             break;
          case H_BLUE:
             h = map(value, 0, 255, 126, 182);
             break;
          case H_PURPLE:
             h = map(value, 0, 255, 182, 200);
             break;
          case H_PINK:
             h = map(value, 0, 255, 200, 237);
             break;
          default:
             h = 0;
             break;
        }

        return CHSV(h, 255, 255);
    }
};
