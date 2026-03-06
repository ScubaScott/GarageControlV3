/**
 * @file menu_callbacks.cpp
 * @brief Menu action handlers for the Smart Garage Controller UI.
 *
 * @details
 * This module implements the callback functions invoked when the user selects
 * a menu item. These callbacks perform actions such as:
 * - Toggling lights
 * - Triggering a door pulse
 * - Changing heater mode
 * - Editing configuration values
 * - Exiting the menu
 *
 * Rendering of menu screens is handled in menus.cpp.
 * UI state transitions are handled in ui_state.cpp.
 *
 * @ingroup UI
 */

#include "menu_callbacks.h"
#include "ui_state.h"
#include "control.h"
#include "sensors.h"
#include <Arduino.h>

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

/**
 * @brief Enters EDIT mode for modifying a configuration value.
 */
static void enterEditMode() {
    g_uiState.screen  = UI_EDIT;
    g_uiState.editing = true;
}

/**
 * @brief Leaves EDIT mode and returns to the menu.
 */
static void exitEditMode() {
    g_uiState.screen  = UI_MENU;
    g_uiState.editing = false;
}

// ---------------------------------------------------------------------------
// Menu Callbacks
// ---------------------------------------------------------------------------

/**
 * @brief Toggles the lights manually.
 *
 * @details
 * This uses the control subsystem's manual override mechanism.
 */
void menuToggleLights() {
    controlToggleLightsManual();
    g_uiState.screen = UI_STATUS;  // Return to status screen
}

/**
 * @brief Sends a door pulse request to the control subsystem.
 */
void menuPulseDoor() {
    controlRequestDoorPulse();
    g_uiState.screen = UI_STATUS;
}

/**
 * @brief Cycles through heater modes (OFF → AUTO → MANUAL → OFF).
 */
void menuHeaterMode() {
    SystemConfig cfg = controlGetConfig();

    switch (cfg.heatMode) {
        case HEAT_OFF:   cfg.heatMode = HEAT_AUTO;   break;
        case HEAT_AUTO:  cfg.heatMode = HEAT_MANUAL; break;
        case HEAT_MANUAL: cfg.heatMode = HEAT_OFF;   break;
    }

    controlSetConfig(cfg);
    g_uiState.screen = UI_STATUS;
}

/**
 * @brief Begins editing the "heat on" temperature threshold.
 */
void menuSetHeatOn() {
    enterEditMode();
}

/**
 * @brief Begins editing the "heat off" temperature threshold.
 */
void menuSetHeatOff() {
    enterEditMode();
}

/**
 * @brief Exits the menu and returns to the main status screen.
 */
void menuExit() {
    g_uiState.screen = UI_STATUS;
}