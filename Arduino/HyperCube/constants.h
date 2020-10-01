#pragma once

// LEDS config
#define NUM_LEDS_SIDE 12
#define NUM_LEDS 4 * NUM_LEDS_SIDE
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define MAX_MILLI_AMPS 8000
#define LED_PIN 6

// Button config
#define BUTTON_PIN A0
#define BUTTON_THRES 2

// BLE config
#define BLE_RXD 2
#define BLE_TXD 3
#define BLE_BAUD 9600
#define BLE_CTRL_CHAR '#'

// EEPROM config
#define ADDR_BRIGHT 0
#define ADDR_MODE 1
#define ADDR_COLOR 2
#define ADDR_SPEED 3
#define ADDR_PATTERN 64

// Other config
#define BRIGHTNESS_STEP 16
#define BRIGHTNESS_BASE 127
#define SPEED_STEP 1
#define SPEED_BASE 8

// longest command "PATTERN A=1 Z=10 C=" + stops * 3
#define CMD_BUFFER_SIZE 19 + 16 * 3
