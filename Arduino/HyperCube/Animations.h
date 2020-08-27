#pragma once

#include <FastLED.h>

#include "constants.h"
#include "modes.h"
#include "hues.h"

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
    // target palette for blending
    CRGBPalette16 targetPalette;
    // palette offset
    int paletteIndex;
    // static color
    CRGB staticColor;

    ColorGenerator colorGenerator;

    void run(boolean isStop) {
      if (isStop) {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        return;
      }

      switch (mode) {
        case M_RAINBOW:
          paletteIndex = paletteIndex + 1;
          displayPalette();
          break;
          
        case M_RAINDROP:
          runRaindrop();
          break;

        case M_CYLON:
          runCylon();
          break;

        case M_PULSE:
          runPulse();
          break;

        case M_RANDOM:
          runRandom();
          break;

        case M_STATIC:
          fill_solid(leds, NUM_LEDS, staticColor);
          break;
      }
    }

    void setCurrentMode(enum Modes newMode) {
      mode = newMode;
      period = 10;
      blending = LINEARBLEND;
      paletteIndex = 0;
      
      colorGenerator.reset();
      
      switch (mode) {
        case M_RAINBOW:
          palette = RainbowColors_p;
          break;

        case M_RAINDROP:
        case M_CYLON:
        case M_PULSE:
          period = 70;
          fill_solid(leds, NUM_LEDS, CRGB::Black);
          break;

        case M_RANDOM:
          palette = CRGBPalette16(CRGB::Black, CRGB::Black);
          CRGB color = colorGenerator.random();
          targetPalette = CRGBPalette16(color, color);
          break;
      }
    }

    void setHue(enum Hues newHue) {
      colorGenerator.setHue(newHue);
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
      CRGB color;
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
        leds[drops[i].pos] += drops[i].color;
        i++;
      }
    }

    void runPulse() {
      static int nbDrops = 0;
      static Drop drops[2];

      // new drop every once
      EVERY_N_MILLIS(350) {
        if (nbDrops < 2) {
          Drop newDrop = {
            .color = colorGenerator.next(20),
            .dir = false,
            .pos = -1,
          };
          drops[nbDrops] = newDrop;
          nbDrops++;
        }
      }

      fadeSide(180);
      showDrops(drops, &nbDrops);
      symetrizeSides();
    }

    void runCylon() {
      static Drop drop = {
        .color = CRGB::Black,
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

      leds[drop.pos] = colorGenerator.next(1);
      symetrizeSides();
    }

    void runRaindrop() {
      static int nbDrops = 0;
      static Drop drops[4];

      // randomly add a new drop
      if (nbDrops < 4) {
        int rnd = random8();
        if (rnd < 48) {
          Drop newDrop = {
            .color = colorGenerator.random(),
            .dir = rnd < 24,
            .pos = rnd < 24 ? NUM_LEDS_SIDE : -1,
          };
          drops[nbDrops] = newDrop;
          nbDrops++;
        }
      }

      fadeSide(150);
      showDrops(drops, &nbDrops);
      symetrizeSides();
    }

    void runRandom() {
      nblendPaletteTowardPalette(palette, targetPalette, 24);

      EVERY_N_SECONDS(5) {
        CRGB color = colorGenerator.random();
        targetPalette = CRGBPalette16(color, color);
      }

      displayPaletteOnSide();
      symetrizeSides();
    }

};
