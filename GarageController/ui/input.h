#pragma once
#include <stdint.h>

enum ButtonEvent {
    BTN_NONE,
    BTN_UP_SHORT,
    BTN_UP_LONG,
    BTN_DOWN_SHORT,
    BTN_DOWN_LONG,
    BTN_SELECT_SHORT,
    BTN_SELECT_LONG
};

void inputInit();
ButtonEvent pollButtons();