/**
 * @file menus.cpp
 * @brief Menu definitions and LCD rendering for the Smart Garage Controller UI.
 *
 * @details
 * This module contains:
 * - The list of menu items (labels + callbacks)
 * - Rendering functions for:
 *      - Status screen
 *      - Menu screen
 *      - Edit screen (if applicable)
 *
 * It does NOT contain the callback implementations themselves; those are located in:
 * - menu_callbacks.cpp
 *
 * The UI state machine (ui_state.cpp) determines which screen to show.
 * This file only handles drawing the appropriate content to the LCD.
 *
 * @ingroup UI
 */

#include "menus.h"
#include "ui_state.h"
#include "ui.h"
#include "menu_callbacks.h"
#include "sensors.h"
#include "control.h"
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

// ---------------------------------------------------------------------------
// LCD Instance
// ---------------------------------------------------------------------------

/** @brief 20x4 I²C LCD instance at address 0x27. */
static LiquidCrystal_I2C lcd(0x27, 20, 4);

// ---------------------------------------------------------------------------
// Menu Definitions
// ---------------------------------------------------------------------------

/**
 * @brief List of menu items displayed in MENU mode.
 *
 * @details
 * Each entry has:
 * - A label shown on the LCD
 * - A callback invoked when SELECT is pressed
 */
MenuItem g_menuItems[] = {
    { "Toggle Lights",     menuToggleLights },
    { "Pulse Door",        menuPulseDoor },
    { "Heater Mode",       menuHeaterMode },
    { "Set Heat On Temp",  menuSetHeatOn },
    { "Set Heat Off Temp", menuSetHeatOff },
    { "Exit Menu",         menuExit }
};

/** @brief Number of menu items. */
static const uint8_t MENU_COUNT = sizeof(g_menuItems) / sizeof(MenuItem);

// ---------------------------------------------------------------------------
// Forward Declarations
// ---------------------------------------------------------------------------

static void drawStatusScreen();
static void drawMenuScreen();
static void drawEditScreen();

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

/**
 * @brief Initializes the LCD and prepares UI rendering.
 */
void menusInit() {
    lcd.init();
    lcd.backlight();
    lcd.clear();
}

/**
 * @brief Renders the appropriate screen based on UI state.
 */
void menusRender() {
    switch (g_uiState.screen) {
        case UI_STATUS: drawStatusScreen(); break;
        case UI_MENU:   drawMenuScreen();   break;
        case UI_EDIT:   drawEditScreen();   break;
    }
}

// ---------------------------------------------------------------------------
// Screen Rendering
// ---------------------------------------------------------------------------

/**
 * @brief Renders the main STATUS screen.
 *
 * @details
 * Shows:
 * - Temperature
 * - Heater state
 * - Door state
 * - Lights state
 */
static void drawStatusScreen() {
    SensorState s = sensorsGetState();
    ActuatorState a = controlGetActuators();

    lcd.clear();

    // Line 1: Temperature
    lcd.setCursor(0, 0);
    if (s.temperatureValid) {
        lcd.print("Temp: ");
        lcd.print(s.temperatureC, 1);
        lcd.print(" C");
    } else {
        lcd.print("Temp: ---");
    }

    // Line 2: Heater
    lcd.setCursor(0, 1);
    lcd.print("Heater: ");
    lcd.print(a.heaterOn ? "ON " : "OFF");

    // Line 3: Door
    lcd.setCursor(0, 2);
    lcd.print("Door: ");
    if (s.doorError) {
        lcd.print("ERROR");
    } else if (s.doorOpen) {
        lcd.print("OPEN ");
    } else if (s.doorClosed) {
        lcd.print("CLOSED");
    } else {
        lcd.print("MOVING");
    }

    // Line 4: Lights
    lcd.setCursor(0, 3);
    lcd.print("Lights: ");
    lcd.print(a.lightsOn ? "ON" : "OFF");
}

/**
 * @brief Renders the MENU screen.
 *
 * @details
 * Shows:
 * - Current menu item
 * - Up/down navigation arrows
 */
static void drawMenuScreen() {
    lcd.clear();

    // Clamp menu index
    if (g_uiState.menuIndex >= MENU_COUNT) {
        g_uiState.menuIndex = MENU_COUNT - 1;
    }

    // Display selected menu item
    lcd.setCursor(0, 0);
    lcd.print("MENU:");

    lcd.setCursor(0, 1);
    lcd.print("> ");
    lcd.print(g_menuItems[g_uiState.menuIndex].label);

    // Navigation hints
    lcd.setCursor(0, 3);
    lcd.print("UP/DOWN to scroll");
}

/**
 * @brief Renders the EDIT screen.
 *
 * @details
 * The actual value editing is handled in menu_callbacks.cpp.
 * This screen simply displays a placeholder.
 */
static void drawEditScreen() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Editing...");

    lcd.setCursor(0, 2);
    lcd.print("UP/DOWN to change");

    lcd.setCursor(0, 3);
    lcd.print("SELECT to save");
}