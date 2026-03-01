#include "render.h"
#include <LiquidCrystal_I2C.h>

extern LiquidCrystal_I2C lcd;

void renderUi(const UiContext& ui) {
    switch(ui.state) {
        case UI_MAIN: renderMainScreen(); break;
        case UI_MENU_ROOT:
        case UI_MENU_SUBMENU: renderMenu(ui); break;
        case UI_EDIT_VALUE: renderEditValue(ui); break;
        case UI_CONFIRM: renderConfirm(ui); break;
        case UI_STATUS_PAGES: renderStatusPages(ui); break;
    }
}

void renderMainScreen() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Temp: --F   Heat:--");
    lcd.setCursor(0,1);
    lcd.print("Door: ---   Lights:--");
    lcd.setCursor(0,2);
    lcd.print("Last Motion: --:--");
    lcd.setCursor(0,3);
    lcd.print("Hold UP=Door DN=Light");
}

void renderMenu(const UiContext& ui) {
    lcd.clear();
    for (int r = 0; r < 4; r++) {
        uint8_t idx = ui.nav.windowStart + r;
        if (idx >= ui.currentMenu->itemCount) break;

        lcd.setCursor(0, r);
        lcd.print(idx == ui.nav.cursorIndex ? ">" : " ");
        lcd.print(ui.currentMenu->items[idx].label);
    }
}

void renderEditValue(const UiContext& ui) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(ui.edit.label);
    lcd.setCursor(0,1);
    lcd.print("[ ");
    lcd.print(*ui.edit.valuePtr);
    lcd.print(" ]");
    lcd.setCursor(0,3);
    lcd.print("UP/DN adjust SEL save");
}

void renderConfirm(const UiContext& ui) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(ui.confirm.prompt);
    lcd.setCursor(0,2);
    lcd.print("[ YES ]   NO");
}

void renderStatusPages(const UiContext& ui) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Status TBD");
}