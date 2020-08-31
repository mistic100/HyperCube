#pragma once

enum Modes {
  M_RAINBOW,
  M_RAINDROP,
  M_PULSE,
  M_CYLON,
  M_CHASER,
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
  if (strcasecmp_P(str, PSTR("CHASER")) == 0) {
    return M_CHASER;
  }
  if (strcasecmp_P(str, PSTR("RANDOM")) == 0) {
    return M_RANDOM;
  }
  return M_RAINBOW;
}

String getModeStr(enum Modes mode) {
  switch (mode) {
    case M_RAINBOW:
      return F("RAINBOW");
    case M_RAINDROP:
      return F("RAINDROP");
    case M_PULSE:
      return F("PULSE");
    case M_CYLON:
      return F("CYLON");
    case M_CHASER:
      return F("CHASER");
    case M_RANDOM:
      return F("RANDOM");
    default:
      return F("UNKNOWN");
  }
}
