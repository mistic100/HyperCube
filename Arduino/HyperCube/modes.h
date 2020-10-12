#pragma once

enum Modes {
  M_RAINBOW,
  M_RAINDROP,
  M_PULSE,
  M_CYLON,
  M_CHASER,
  M_NOISE,
  M_RIPPLES,
  M_GRADIENT,
  NUM_MODES,
  M_PATTERN // not accessible through loop
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
  if (strcasecmp_P(str, PSTR("NOISE")) == 0) {
    return M_NOISE;
  }
  if (strcasecmp_P(str, PSTR("GRADIENT")) == 0) {
    return M_GRADIENT;
  }
  if (strcasecmp_P(str, PSTR("RIPPLES")) == 0) {
    return M_RIPPLES;
  }
  if (strcasecmp_P(str, PSTR("PATTERN")) == 0) {
    return M_PATTERN;
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
    case M_NOISE:
      return F("NOISE");
    case M_GRADIENT:
      return F("GRADIENT");
    case M_RIPPLES:
      return F("RIPPLES");
    case M_PATTERN:
      return F("PATTERN");
    default:
      return F("UNKNOWN");
  }
}
