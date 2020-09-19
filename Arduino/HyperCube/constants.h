#pragma once

// LEDS config
#define NUM_LEDS_SIDE 12
#define NUM_LEDS 4 * NUM_LEDS_SIDE
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define MAX_MILLI_AMPS 8000
#define PIN_LED 6

// Button config
#define PIN_BUTTON A0
#define THRES_BUTTON 1

// BLE config
#define BLE_RXD 2
#define BLE_TXD 3

// EEPROM config
#define BRIGHT_ADDR 0
#define MODE_ADDR 1
#define COLOR_ADDR 2
#define SPEED_ADDR 3
#define PATTERN_ADDR 64

// Other config
#define BRIGHTNESS_STEP 16
#define BRIGHTNESS_BASE 127
#define SPEED_STEP 1
#define SPEED_BASE 8

// longest command "PATTERN A=1 Z=10 C=" + stops * 3
#define CMD_BUFFER_SIZE 19 + 16 * 3
