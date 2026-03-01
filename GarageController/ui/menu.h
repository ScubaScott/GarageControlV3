#pragma once
#include <stdint.h>
#include "input.h"

struct MenuItem {
    const char* label;
    void (*onSelect)();
    bool editable;
};

struct Menu {
    const MenuItem* items;
    uint8_t itemCount;
};

struct MenuNav {
    uint8_t cursorIndex;
    uint8_t windowStart;
};

void handleMenuNavigation(ButtonEvent e, UiContext& ui);