#pragma once

#include <FastLED.h>
#include "constants.h"
#include "modes.h"
#include "colors.h"
#include "functions.h"
#include "custom.h"

struct Drop {
  CRGB color;
  int step;
  int pos;
};

class Animations {

  public:
    // leds
    CRGB leds[NUM_LEDS];
    // current animation
    enum Modes mode = M_RAINBOW;
    // current color
    enum Colors color = H_RAINBOW;
    // current speed
    uint8_t speed = SPEED_BASE;
    // current brightness
    uint8_t brightness = BRIGHTNESS_BASE;
    // on/off state
    boolean isOff = false;
    // pattern
    CustomPattern pattern;
    // loop period in ms
    int period;
    // displayed palette
    CRGBPalette16 palette;
    // target palette for blending
    CRGBPalette16 targetPalette;
    // palette offset
    uint8_t paletteIndex;
    // color index
    uint8_t colorIndex;

    void off() {
      isOff = true;
      fill_solid(leds, NUM_LEDS, CRGB::Black);
    }

    void on() {
      isOff = false;
    }

    void run() {
      if (isOff) {
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

        case M_RIPPLES:
          runRipples();
          break;

        case M_PATTERN:
          runPattern();
          break;
      }
    }

    void setMode(enum Modes newMode) {
      mode = newMode;
      paletteIndex = 0;
      colorIndex = 0;

      setBrightness(brightness);
      setSpeed(speed);
      setColor(color);
      
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      
      switch (mode) {
        case M_GRADIENT:
          palette = CRGBPalette16(CRGB::Black);
          break;

        case M_PATTERN:
          if (pattern.nbStops == 0) {
            palette = CRGBPalette16(CRGB::Black);
          }
          else if (pattern.nbStops == 1) {
            palette = CRGBPalette16(pattern.stops[0], pattern.stops[0]);
          }
          else {
            // building CRGBPalette16 with variable number of stops
            palette = CRGBPalette16();
            
            double step = 16.0 / (pattern.nbStops - 1);
            double previous = 0.0;
            double next = 0.0;
      
            for (uint8_t i = 1; i < pattern.nbStops; i++) {
              next+= step;
              fill_gradient_RGB(&(palette.entries[0]), previous, pattern.stops[i - 1], next, pattern.stops[i]);
              previous = next;
            }
          }
          break;
      }
    }

    void setColor(enum Colors newColor) {
      color = newColor;

      switch (mode) {
        case M_RAINBOW:
          palette = getPalette(color);
          break;

        case M_GRADIENT:
          targetPalette = CRGBPalette16(randomColor(), randomColor());
          break;
      }
    }

    void setSpeed(uint8_t newSpeed) {
      speed = max(min(newSpeed, 10), 1);
      // maps 1-10 to 1024-2
      period = 1 << (11 - newSpeed);

      switch (mode) {
        case M_RAINDROP:
        case M_CYLON:
        case M_PULSE:
          period *= 8;
          break;
          
        case M_CHASER:
          period *= 6;
          break;

        case M_RIPPLES:
          period *= 10;
          break;
      }
    }

    void setBrightness(uint8_t newBrightness) {
      brightness = newBrightness;
    }

  private:
    /** UTILITIES */

    void showPalette(uint8_t endIndex, uint8_t scale = 255) {
      for (uint8_t i = 0; i < endIndex; i++) {
        // 240 is 16x15, hi-bit is 15, low-bit is 0, this prevents ColorFromPalette to perform blending with the first color
        uint8_t index = min(1.0 * scale * i / (endIndex - 1), 240);
        leds[i] = ColorFromPalette(palette, index + paletteIndex);
      }
    }

    void symetrizeSides() {
      for (uint8_t i = 0; i < NUM_LEDS_SIDE; i++) {
        leds[NUM_LEDS_SIDE * 2 - 1 - i] = leds[i];
        leds[NUM_LEDS_SIDE * 2 + i] = leds[i];
        leds[NUM_LEDS_SIDE * 4 - 1 - i] = leds[i];
      }
    }

    void duplicateSides() {
      for (uint8_t i = 0; i < NUM_LEDS_SIDE; i++) {
        leds[NUM_LEDS_SIDE + i] = leds[i];
        leds[NUM_LEDS_SIDE * 2 + i] = leds[i];
        leds[NUM_LEDS_SIDE * 3 + i] = leds[i];
      }
    }

    void fade(uint8_t fade, uint8_t endIndex) {
      for (uint8_t i = 0; i < endIndex; i++) {
        leds[i].nscale8(fade);
      }
    }

    CRGB nextColor(uint8_t inc) {
      colorIndex+= inc;
      return colorFromPalette(colorIndex);
    }

    CRGB randomColor() {
      return colorFromPalette(random8());
    }

    CRGB colorFromPalette(uint8_t value) {
      return ColorFromPalette(getPalette(color), min(value, 240));
    }

    int wrapIdx(int i) {
      if (i < 0) {
        return NUM_LEDS + i;
      } else if (i >= NUM_LEDS) {
        return i - NUM_LEDS;
      } else {
        return i;
      }
    }

    void showDrops(Drop drops[], uint8_t *nbDrops, uint8_t endIndex) {
      for (uint8_t i = 0; i < *nbDrops;) {
        // move each drop
        if (drops[i].step == 1) {
          drops[i].pos--;
        } else {
          drops[i].pos++;
        }

        // remove drops on ends
        if (drops[i].pos == -1 || drops[i].pos == endIndex) {
          for (uint8_t j=i; j<*nbDrops-1; j++) {
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
      paletteIndex+= 1;
      
      showPalette(NUM_LEDS_SIDE);
      symetrizeSides();
    }

    void runPulse() {
      static uint8_t nbDrops = 0;
      static boolean run = true;
      static Drop drops[2];

      if (nbDrops == 0 || drops[0].pos == 5) {
        Drop newDrop = {
          .color = nextColor(20),
          .step = 0,
          .pos = -1,
        };
        drops[nbDrops] = newDrop;
        nbDrops++;
      }

      fade(180, NUM_LEDS_SIDE);
      showDrops(drops, &nbDrops, NUM_LEDS_SIDE);
      symetrizeSides();
    }

    void runCylon() {
      static Drop drop = {
        .color = CRGB::Black,
        .step = 0,
        .pos = -1,
      };

      fade(100, NUM_LEDS_SIDE);

      if (drop.step == 1) {
        drop.pos--;
        if (drop.pos == -1) {
          drop.step = 0;
          drop.pos += 2;
        }
      } else {
        drop.pos++;
        if (drop.pos == NUM_LEDS_SIDE) {
          drop.step = 1;
          drop.pos -= 2;
        }
      }

      leds[drop.pos] = nextColor(1);
      symetrizeSides();
    }

    void runRaindrop() {
      static uint8_t nbDrops = 0;
      static Drop drops[4];

      // randomly add a new drop
      if (nbDrops < 4) {
        uint8_t rnd = random8();
        if (rnd < 48) {
          Drop newDrop = {
            .color = randomColor(),
            .step = rnd < 24 ? 1 : 0,
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
        .step = 0,
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

      EVERY_N_MILLIS_I(timer, period * 4) {
        timer.setPeriod(period * 4);
        leds[random8(0, NUM_LEDS)] += randomColor();
      }
    }

    void runGradient() {
      static bool odd = false;
  
      if (odd) {
        nblendPaletteTowardPaletteReverse(palette, targetPalette, 12);
      } else {
        nblendPaletteTowardPalette(palette, targetPalette, 12);
      }
  
      EVERY_N_MILLIS_I(timer, period * 400) {
        timer.setPeriod(period * 400);
  
        odd = !odd;
        
        if (odd) {
          targetPalette = CRGBPalette16(randomColor(), palette[15]);
        }
        else {
          targetPalette = CRGBPalette16(palette[0], randomColor());
        }
      }

      showPalette(NUM_LEDS_SIDE);
      symetrizeSides();
    }

    void runRipples() {
      static uint8_t nbDrops = 0;
      static Drop drops[4];

      if (nbDrops < 4) {
        if (random8() < 64) {
          Drop newDrop = {
            .color = nextColor(2),
            .step = 0,
            .pos = random8(NUM_LEDS),
          };
          drops[nbDrops] = newDrop;
          nbDrops++;
        }
      }

      fade(150, NUM_LEDS);

      for (uint8_t i = 0; i < nbDrops;) {
        if (drops[i].step == 0) {
          leds[drops[i].pos] = drops[i].color;
        }
        else {
          leds[wrapIdx(drops[i].pos + drops[i].step)]+= drops[i].color;
          leds[wrapIdx(drops[i].pos - drops[i].step)]+= drops[i].color;
          drops[i].color.nscale8(200);
        }
        drops[i].step++;

        if (drops[i].step == 10) {
          // remove drop
          for (uint8_t j=i; j<nbDrops-1; j++) {
              drops[j] = drops[j + 1];
          }
          nbDrops--;
          continue;
        }

        i++;
      }
    }

    void runPattern() {
      switch (pattern.anim) {
        case A_FORWARD:
          paletteIndex++;
          break;
        case A_BACKWARD:
          paletteIndex--;
          break;
      }

      // special case when exactly one color for each LED
      if (pattern.nbStops == NUM_LEDS_SIDE && pattern.zoom == 1) {
        for (uint8_t i = 0; i < NUM_LEDS_SIDE; i++) {
          leds[i] = pattern.stops[(i + paletteIndex) % NUM_LEDS_SIDE];
        }
      } else {
        uint8_t scale = changeScale(pattern.zoom, 10, 1, 1, 255);
        showPalette(NUM_LEDS_SIDE, scale);
      }
      symetrizeSides();
    }

};
