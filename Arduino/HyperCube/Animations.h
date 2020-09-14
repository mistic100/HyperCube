#pragma once

#include <FastLED.h>
#include "constants.h"
#include "modes.h"
#include "hues.h"
#include "functions.h"

struct Drop {
  CRGB color;
  bool dir;
  int pos;
};

class Animations {

  public:
    // leds
    CRGB leds[NUM_LEDS];
    // current animation
    enum Modes mode = M_RAINBOW;
    // current hue
    enum Hues hue = H_RAINBOW;
    // current speed
    int speed = 5;
    // loop period in ms
    int period;
    // displayed palette
    CRGBPalette16 palette;
    // target palette for blending
    CRGBPalette16 targetPalette;
    // palette offset
    int paletteIndex;
    // color index
    int colorIndex;

    void run(boolean isStop) {
      if (isStop) {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        return;
      }

      switch (mode) {
        case M_RAINBOW:
          runRainbow();
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

        case M_CHASER:
          runChaser();
          break;

        case M_NOISE:
          runNoise();
          break;

        case M_GRADIENT:
          runGradient();
          break;
      }
    }

    void setMode(enum Modes newMode) {
      mode = newMode;
      period = 20 * speed / 10.0;
      paletteIndex = 0;
      colorIndex = 0;
      
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      
      switch (mode) {
        case M_RAINBOW:
          palette = getPalette(hue);
          break;
        
        case M_RAINDROP:
        case M_CYLON:
        case M_PULSE:
          period *= 7;
          break;
          
        case M_CHASER:
          period *= 5;
          break;

        case M_GRADIENT:
          palette = CRGBPalette16(CRGB::Black, CRGB::Black);
          targetPalette = CRGBPalette16(randomColor(), randomColor());
          break;
      }
    }

    void setHue(enum Hues newHue) {
      hue = newHue;

      switch (mode) {
        case M_RAINBOW:
          palette = getPalette(hue);
          break;

        case M_GRADIENT:
          targetPalette = CRGBPalette16(randomColor(), randomColor());
          break;
      }
    }

    void setSpeed(double newSpeed) {
      speed = max(min(newSpeed, SPEED_MAX), SPEED_MIN);
      period = 20 * speed / 10.0;

      switch (mode) {
        case M_RAINDROP:
        case M_CYLON:
        case M_PULSE:
          period *= 7;
          break;
          
        case M_CHASER:
          period *= 5;
          break;
      }
    }

  private:
    /** UTILITIES */

    void showPalette(int endIndex) {
      for (int i = 0; i < endIndex; i++) {
        uint8_t index = 255.0 * i / endIndex;
        leds[i] = ColorFromPalette(palette, index + paletteIndex, 255, LINEARBLEND);
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

    void fade(int fade, int endIndex) {
      for (int i = 0; i < endIndex; i++) {
        leds[i].nscale8(fade);
      }
    }

    CRGB nextColor(int inc) {
      colorIndex+= inc;

      if (colorIndex >= 255) {
        colorIndex-= 255;
      }
      
      return colorFromPalette(colorIndex);
    }

    CRGB randomColor() {
      return colorFromPalette(random8());
    }

    CRGB colorFromPalette(int value) {
      return ColorFromPalette(getPalette(hue), value, 255, LINEARBLEND);
    }

    void showDrops(Drop drops[], int *nbDrops, int endIndex) {
      for (int i = 0; i < *nbDrops;) {
        // move each drop
        if (drops[i].dir) {
          drops[i].pos--;
        } else {
          drops[i].pos++;
        }

        // remove drops on ends
        if (drops[i].pos == -1 || drops[i].pos == endIndex) {
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

    /** ANIMATIONS */

    void runRainbow() {
      paletteIndex = paletteIndex + 1;
      
      showPalette(NUM_LEDS_SIDE);
      symetrizeSides();
    }

    void runPulse() {
      static int nbDrops = 0;
      static Drop drops[2];

      EVERY_N_MILLIS_I(timer, 350) {
        timer.setPeriod(700 * speed / 10.0);
        
        if (nbDrops < 2) {
          Drop newDrop = {
            .color = nextColor(20),
            .dir = false,
            .pos = -1,
          };
          drops[nbDrops] = newDrop;
          nbDrops++;
        }
      }

      fade(180, NUM_LEDS_SIDE);
      showDrops(drops, &nbDrops, NUM_LEDS_SIDE);
      symetrizeSides();
    }

    void runCylon() {
      static Drop drop = {
        .color = CRGB::Black,
        .dir = false,
        .pos = -1,
      };

      fade(100, NUM_LEDS_SIDE);

      if (drop.dir) {
        drop.pos--;
        if (drop.pos == -1) {
          drop.dir = false;
          drop.pos += 2;
        }
      } else {
        drop.pos++;
        if (drop.pos == NUM_LEDS_SIDE) {
          drop.dir = true;
          drop.pos -= 2;
        }
      }

      leds[drop.pos] = nextColor(1);
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
            .color = randomColor(),
            .dir = rnd < 24,
            .pos = rnd < 24 ? NUM_LEDS_SIDE : -1,
          };
          drops[nbDrops] = newDrop;
          nbDrops++;
        }
      }

      fade(150, NUM_LEDS_SIDE);
      showDrops(drops, &nbDrops, NUM_LEDS_SIDE);
      symetrizeSides();
    }

    void runChaser() {
      static Drop drop = {
        .color = CRGB::Black,
        .dir = false,
        .pos = -1,
      };

      fade(150, NUM_LEDS);

      drop.pos++;
      if (drop.pos == NUM_LEDS) {
        drop.pos = 0;
      }

      leds[drop.pos] = nextColor(1);
    }

    void runNoise() {
      fade(250, NUM_LEDS);

      EVERY_N_MILLIS_I(timer, 30) {
        timer.setPeriod(60 * speed / 10.0);
        leds[random8(0, NUM_LEDS)] += randomColor();
      }
    }

    void runGradient() {
      static bool odd = false;
  
      if (odd) {
        nblendPaletteTowardPalette(palette, targetPalette, 12);
      } else {
        nblendPaletteTowardPaletteReverse(palette, targetPalette, 12);
      }
  
      EVERY_N_MILLIS_I(timer, 2000) {
        timer.setPeriod(4000 * speed / 10.0);
        
        if (odd) {
          targetPalette = CRGBPalette16(palette[0], randomColor());
        }
        else {
          targetPalette = CRGBPalette16(randomColor(), palette[15]);
        }
  
        odd = !odd;
      }
  
      showPalette(NUM_LEDS_SIDE);
      symetrizeSides();
    }

};
