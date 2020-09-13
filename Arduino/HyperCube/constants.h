#pragma once

// LEDS config
#define NUM_LEDS_SIDE 12
#define NUM_LEDS 4 * NUM_LEDS_SIDE
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define MAX_MILLI_AMPS 8000
#define PIN_LED 6
#define BRIGHTNESS_STEP 16
#define SPEED_STEP 0.1
#define SPEED_MIN 0.1
#define SPEED_MAX 0.9

// Button config
#define PIN_BUTTON A0
#define THRES_BUTTON 1

// BLE config
#define BLE_RXD 2
#define BLE_TXD 3

// EEPROM config
#define BRIGHT_ADDR 0
#define MODE_ADDR 1
#define HUE_ADDR 2
#define SPEED_ADDR 3
