#include "menu.h"
#include "ui_state.h"

void handleMenuNavigation(ButtonEvent e, UiContext& ui) {
    Menu* m = ui.currentMenu;

    switch(e) {

        case BTN_UP_SHORT:
            if (ui.nav.cursorIndex > 0) {
                ui.nav.cursorIndex--;
                if (ui.nav.cursorIndex < ui.nav.windowStart)
                    ui.nav.windowStart--;
                ui.lcdNeedsUpdate = true;
            }
            break;

        case BTN_DOWN_SHORT:
            if (ui.nav.cursorIndex < m->itemCount - 1) {
                ui.nav.cursorIndex++;
                if (ui.nav.cursorIndex > ui.nav.windowStart + 3)
                    ui.nav.windowStart++;
                ui.lcdNeedsUpdate = true;
            }
            break;

        case BTN_SELECT_SHORT: {
            const MenuItem& item = m->items[ui.nav.cursorIndex];
            item.onSelect();
            ui.lcdNeedsUpdate = true;
            break;
        }

        case BTN_SELECT_LONG:
            ui.state = UI_MAIN;
            ui.lcdNeedsUpdate = true;
            break;

        default:
            break;
    }
}