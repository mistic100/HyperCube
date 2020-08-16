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
          
        case RAINBOW_DROP:
          runRainbowDrop();
          symetrizeSides();
          break;

        case CYLON:
          runCylon();
          symetrizeSides();
          break;

        case PULSE:
          runPulse();
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

        case RAINBOW_DROP:
        case CYLON:
        case PULSE:
          period = 50;
          fill_solid(leds, NUM_LEDS, CRGB::Black);
          break;
  
        case STATIC:
          break;
      }
    }

  private:

    void displayPalette() {
      for (int i = 0; i < NUM_LEDS; i++) {
        uint8_t index = 255.0 * i / NUM_LEDS;
        leds[i] = ColorFromPalette(palette, index + paletteIndex, 255, blending);
      }
    }

    void displayPaletteOnSide() {
      for (int i = 0; i < NUM_LEDS_SIDE; i++) {
        uint8_t index = 255.0 * i / NUM_LEDS_SIDE;
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

    void fadeSide(int fade) {
      for (int i = 0; i < NUM_LEDS_SIDE; i++) {
        leds[i].nscale8(fade);
      }
    }

    struct Drop {
      int hue;
      bool dir;
      int pos;
    };

    void showDrops(Drop drops[], int *nbDrops) {
      for (int i = 0; i < *nbDrops;) {
        // move each drop
        if (drops[i].dir) {
          drops[i].pos--;
        } else {
          drops[i].pos++;
        }

        // remove drops on ends
        if (drops[i].pos == -1 || drops[i].pos == NUM_LEDS_SIDE) {
          for (int j=i; j<*nbDrops-1; j++) {
              drops[j] = drops[j + 1];
          }
          (*nbDrops)--;
          continue;
        }

        // display the drop
        leds[drops[i].pos] += CHSV(drops[i].hue, 255, 255);
        i++;
      }
    }

    void runPulse() {
      static int hue = 0;
      static int nbDrops = 0;
      static Drop drops[2];

      // new drop every once
      EVERY_N_MILLIS(500) {
        if (nbDrops < 2) {
          Drop newDrop = {
            .hue = hue-=20,
            .dir = false,
            .pos = -1,
          };
          drops[nbDrops] = newDrop;
          nbDrops++;
        }
      }

      fadeSide(180);

      showDrops(drops, &nbDrops);
    }

    void runCylon() {
      static Drop drop = {
        .hue = 0,
        .dir = false,
        .pos = -1,
      };

      fadeSide(100);

      if (drop.dir) {
        drop.pos--;
        if (drop.pos == -1) {
          drop.dir = false;
          drop.pos = 1;
        }
      } else {
        drop.pos++;
        if (drop.pos == NUM_LEDS_SIDE) {
          drop.dir = true;
          drop.pos = NUM_LEDS_SIDE - 2;
        }
      }

      drop.hue++;
      leds[drop.pos] = CHSV(drop.hue, 255, 255);
    }

    void runRainbowDrop() {
      static int nbDrops = 0;
      static Drop drops[4];

      // randomly add a new drop
      if (nbDrops < 4) {
        int rnd = random8();
        if (rnd < 32) {
          Drop newDrop = {
            .hue = random8(),
            .dir = rnd < 16,
            .pos = rnd < 16 ? NUM_LEDS_SIDE : -1,
          };
          drops[nbDrops] = newDrop;
          nbDrops++;
        }
      }

      fadeSide(100);

      showDrops(drops, &nbDrops);
    }

};
