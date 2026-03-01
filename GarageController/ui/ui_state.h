#pragma once
#include <stdint.h>
#include "menu.h"
#include "input.h"

enum UiState {
    UI_MAIN,
    UI_MENU_ROOT,
    UI_MENU_SUBMENU,
    UI_EDIT_VALUE,
    UI_CONFIRM,
    UI_STATUS_PAGES
};

struct EditContext {
    const char* label;
    int* valuePtr;
    int minValue;
    int maxValue;
    int step;
};

struct ConfirmContext {
    const char* prompt;
    void (*onConfirm)();
};

struct UiContext {
    UiState state;
    Menu* currentMenu;
    MenuNav nav;
    EditContext edit;
    ConfirmContext confirm;
    uint32_t lastInteractionMs;
    bool lcdNeedsUpdate;
};

void uiStateInit();
void uiStateHandleEvent(ButtonEvent e);
void uiStateLoop();