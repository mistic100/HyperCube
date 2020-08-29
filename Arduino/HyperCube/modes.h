#pragma once

enum Modes {
  M_RAINBOW,
  M_RAINDROP,
  M_PULSE,
  M_CYLON,
  M_RANDOM,
  NUM_MODES
};

enum Modes getMode(char* str) {
  if (strcasecmp_P(str, PSTR("RAINBOW")) == 0) {
    return M_RAINBOW;
  }
  if (strcasecmp_P(str, PSTR("RAINDROP")) == 0) {
    return M_RAINDROP;
  }
  if (strcasecmp_P(str, PSTR("PULSE")) == 0) {
    return M_PULSE;
  }
  if (strcasecmp_P(str, PSTR("CYLON")) == 0) {
    return M_CYLON;
  }
  if (strcasecmp_P(str, PSTR("RANDOM")) == 0) {
    return M_RANDOM;
  }
  return M_RAINBOW;
}
