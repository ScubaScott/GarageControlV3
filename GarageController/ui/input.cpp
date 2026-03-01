#include "input.h"
#include <Arduino.h>

static const uint8_t PIN_UP = 2;
static const uint8_t PIN_DOWN = 3;
static const uint8_t PIN_SELECT = 4;

static uint32_t upStart = 0;
static uint32_t downStart = 0;
static uint32_t selStart = 0;

static const uint16_t LONG_MS = 2000;

void inputInit() {
    pinMode(PIN_UP, INPUT_PULLUP);
    pinMode(PIN_DOWN, INPUT_PULLUP);
    pinMode(PIN_SELECT, INPUT_PULLUP);
}

ButtonEvent pollButtons() {
    // TODO: implement debouncing + long press detection
    return BTN_NONE;
}