#pragma once

#include "constants.h"

#define CUSTOM_PATTERN_ERROR 1

#define CUSTOM_PATTERN_VERSION 4
#define CUSTOM_PATTERN_MAX_STOP 16

enum CustomPatternAnim {
  A_NONE,
  A_FORWARD,
  A_BACKWARD
};

enum CustomPatternAnim getAnim(char* str) {
  if (strcasecmp_P(str, PSTR("F")) == 0) {
    return A_FORWARD;
  }
  if (strcasecmp_P(str, PSTR("B")) == 0) {
    return A_BACKWARD;
  }
  return A_NONE;
}

String getAnimStr(enum CustomPatternAnim anim) {
  switch (anim) {
    case A_FORWARD:
      return F("F");
    case A_BACKWARD:
      return F("B");
    default:
      return F("0");
  }
}

struct CustomPattern {
  uint8_t version;
  uint8_t nbStops;
  CRGB stops[CUSTOM_PATTERN_MAX_STOP];
  CustomPatternAnim anim;
  uint8_t zoom;
  bool hasExtraStop;
};

int parseCustomPattern(CustomPattern* pattern, char* stopsStr, char* animStr, char* zoomStr) {
  if (stopsStr == NULL) {
    return CUSTOM_PATTERN_ERROR;
  }

  pattern->nbStops = 0;
  memset(pattern->stops, 0, CUSTOM_PATTERN_MAX_STOP * sizeof(CRGB));
  
  uint8_t parsedStops = 0;
  int r = -1;
  int g = -1;
  int b = -1;
  
  for (uint8_t i = 0; stopsStr[i] != 0x00; i++) {
    char c[] = { stopsStr[i], stopsStr[i], 0x00 };
    if (r == -1) {
      r = strtol(c, NULL, 16);
    }
    else if (g == -1) {
      g = strtol(c, NULL, 16);
    }
    else if (b == -1) {
      b = strtol(c, NULL, 16);
      pattern->stops[parsedStops++] = CRGB(r, g, b);
      r = g = b = -1;

      if (parsedStops == CUSTOM_PATTERN_MAX_STOP) {
        break;
      }
    }
  }

  if (r != -1 || g != -1 || b != -1 || parsedStops == 0) {
    return CUSTOM_PATTERN_ERROR;
  }

  pattern->version = CUSTOM_PATTERN_VERSION;
  pattern->nbStops = parsedStops;
  pattern->hasExtraStop = false;
  pattern->anim = getAnim(animStr);

  if (zoomStr != NULL) {
    pattern->zoom = max(min(atoi(zoomStr), 10), 1);
  } else {
    pattern->zoom = 1;
  }

  if (pattern->anim != A_NONE && pattern->nbStops < CUSTOM_PATTERN_MAX_STOP) {
    pattern->stops[pattern->nbStops++] = pattern->stops[0];
    pattern->hasExtraStop = true;
  }

  return 0;
}

char* getPatternStr(CustomPattern* pattern) {
  static char buffer[CMD_BUFFER_SIZE];

  memset(buffer, 0, CMD_BUFFER_SIZE);

  int i = 0;
  buffer[i++] = 'A';
  buffer[i++] = '=';
  String animStr = getAnimStr(pattern->anim);
  memcpy(&buffer[i], animStr.c_str(), animStr.length());
  i+= animStr.length();
  buffer[i++] = ' ';
  buffer[i++] = 'Z';
  buffer[i++] = '=';
  String zoomStr = String(pattern->zoom, DEC);
  memcpy(&buffer[i], zoomStr.c_str(), zoomStr.length());
  i+= zoomStr.length();
  buffer[i++] = ' ';
  buffer[i++] = 'C';
  buffer[i++] = '=';

  for (int j = 0; j < pattern->nbStops - (pattern->hasExtraStop ? 1 : 0); j++) {
    buffer[i++] = String(pattern->stops[j].red, HEX).charAt(0);
    buffer[i++] = String(pattern->stops[j].green, HEX).charAt(0);
    buffer[i++] = String(pattern->stops[j].blue, HEX).charAt(0);
  }

  return buffer;
}
