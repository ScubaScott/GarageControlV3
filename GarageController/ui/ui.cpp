#include "ui.h"
#include "ui_state.h"
#include "render.h"
#include "input.h"

void uiInit() {
    uiStateInit();
}

void uiHandleEvent(ButtonEvent e) {
    uiStateHandleEvent(e);
}

void uiLoop() {
    uiStateLoop();
}