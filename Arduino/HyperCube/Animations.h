#pragma once

#include <FastLED.h>

#include "constants.h"
#include "modes.h"

class Animations {

  public:
    // leds
    CRGB leds[NUM_LEDS];
    // loop period in ms
    int period;
    // current animation
    enum Modes mode;
    // current blending mode
    TBlendType blending;
    // displayed palette
    CRGBPalette16 palette;
    // palette offset
    int paletteIndex;
    // static color
    CRGB staticColor;

    void run(boolean isStop) {
      if (isStop) {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        return;
      }

      switch (mode) {
        case RAINBOW:
          paletteIndex = paletteIndex + 1;
          displayPaletteOnSide();
          symetrizeSides();
          break;

        case STATIC:
          fill_solid(leds, NUM_LEDS, staticColor);
          break;
      }
    }

    void setCurrentMode(enum Modes newMode) {
      mode = newMode;
      period = 10;
      blending = LINEARBLEND;
      paletteIndex = 0;
      
      switch (mode) {
        case RAINBOW:
          palette = RainbowColors_p;
          break;
  
        case STATIC:
          break;
      }
    }

  private:

    void displayPalette() {
      for (int i = 0; i < NUM_LEDS; i++) {
        uint8_t index = 1.0 * i / NUM_LEDS;
        leds[i] = ColorFromPalette(palette, index + paletteIndex, 255, blending);
      }
    }

    void displayPaletteOnSide() {
      for (int i = 0; i < NUM_LEDS_SIDE; i++) {
        uint8_t index = 1.0 * i / NUM_LEDS_SIDE;
        leds[i] = ColorFromPalette(palette, index + paletteIndex, 255, blending);
      }
    }

    void symetrizeSides() {
      for (int i = 0; i < NUM_LEDS_SIDE; i++) {
        leds[NUM_LEDS_SIDE * 2 - 1 - i] = leds[i];
        leds[NUM_LEDS_SIDE * 2 + i] = leds[i];
        leds[NUM_LEDS_SIDE * 4 - 1 - i] = leds[i];
      }
    }

    void duplicateSides() {
      for (int i = 0; i < NUM_LEDS_SIDE; i++) {
        leds[NUM_LEDS_SIDE + i] = leds[i];
        leds[NUM_LEDS_SIDE * 2 + i] = leds[i];
        leds[NUM_LEDS_SIDE * 3 + i] = leds[i];
      }
    }
    
};
