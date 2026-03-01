#include "ui_state.h"
#include "render.h"
#include "input.h"
#include "actions.h"
#include <Arduino.h>

static UiContext ui;

void uiStateInit() {
    ui.state = UI_MAIN;
    ui.currentMenu = nullptr;
    ui.nav = {0,0};
    ui.lcdNeedsUpdate = true;
    ui.lastInteractionMs = millis();
}

void uiStateHandleEvent(ButtonEvent e) {
    ui.lastInteractionMs = millis();

    switch(ui.state) {

        case UI_MAIN:
            if (e == BTN_UP_SHORT || e == BTN_SELECT_SHORT) {
                ui.state = UI_MENU_ROOT;
                ui.nav = {0,0};
                ui.lcdNeedsUpdate = true;
            }
            else if (e == BTN_UP_LONG) {
                pulseDoorRelay();
            }
            else if (e == BTN_DOWN_LONG) {
                toggleLights();
            }
            break;

        case UI_MENU_ROOT:
        case UI_MENU_SUBMENU:
            handleMenuNavigation(e, ui);
            break;

        case UI_EDIT_VALUE:
            handleEditValue(e, ui);
            break;

        case UI_CONFIRM:
            handleConfirm(e, ui);
            break;

        case UI_STATUS_PAGES:
            handleStatusPages(e, ui);
            break;
    }
}

void uiStateLoop() {
    if (millis() - ui.lastInteractionMs > 30000) {
        ui.state = UI_MAIN;
        ui.lcdNeedsUpdate = true;
    }

    if (ui.lcdNeedsUpdate) {
        renderUi(ui);
        ui.lcdNeedsUpdate = false;
    }
}