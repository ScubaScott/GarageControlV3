/**
 * @file ui_state.cpp
 * @brief Implementation of the UI state machine for the Smart Garage Controller.
 *
 * @details
 * This module manages the high-level UI state:
 * - Current screen (status, menu, edit)
 * - Menu navigation index
 * - Editing mode
 * - LCD refresh timing
 *
 * It does NOT perform LCD rendering directly. Rendering is handled by:
 * - menus.cpp
 * - menu_callbacks.cpp
 *
 * The UI state machine is driven by:
 * - uiStateHandleEvent() for button input
 * - uiStateUpdate() for periodic refresh
 *
 * @ingroup UI
 */

#include "ui_state.h"
#include "ui.h"
#include <Arduino.h>

// ---------------------------------------------------------------------------
// Global UI state instance
// ---------------------------------------------------------------------------

UIState g_uiState;

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

/** @brief Minimum time between LCD refreshes (ms). */
static const uint32_t UI_REFRESH_INTERVAL_MS = 250;

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void uiStateInit() {
    g_uiState.screen        = UI_STATUS;
    g_uiState.menuIndex     = 0;
    g_uiState.editing       = false;
    g_uiState.lastRefreshMs = millis();
}

/**
 * @brief Handles button events and updates UI state accordingly.
 *
 * @details
 * Behavior by screen:
 *
 * STATUS screen:
 * - UP/DOWN: no effect
 * - SELECT: enter MENU screen
 *
 * MENU screen:
 * - UP: move selection up
 * - DOWN: move selection down
 * - SELECT: invoke menu callback (handled in menus.cpp)
 *
 * EDIT screen:
 * - UP/DOWN: modify value (handled in callbacks)
 * - SELECT: exit edit mode
 *
 * @param ev Button event.
 */
void uiStateHandleEvent(uint8_t ev) {
    switch (g_uiState.screen) {

        // ---------------------------------------------------------------
        // STATUS SCREEN
        // ---------------------------------------------------------------
        case UI_STATUS:
            if (ev == BTN_SELECT) {
                g_uiState.screen = UI_MENU;
                g_uiState.menuIndex = 0;
            }
            break;

        // ---------------------------------------------------------------
        // MENU SCREEN
        // ---------------------------------------------------------------
        case UI_MENU:
            if (ev == BTN_UP) {
                if (g_uiState.menuIndex > 0) {
                    g_uiState.menuIndex--;
                }
            }
            else if (ev == BTN_DOWN) {
                g_uiState.menuIndex++;
            }
            else if (ev == BTN_SELECT) {
                // Actual callback is invoked in menus.cpp
                // UI state machine only changes mode if callback requests it
            }
            break;

        // ---------------------------------------------------------------
        // EDIT SCREEN
        // ---------------------------------------------------------------
        case UI_EDIT:
            if (ev == BTN_SELECT) {
                g_uiState.editing = false;
                g_uiState.screen = UI_MENU;
            }
            // UP/DOWN handled by callback functions
            break;
    }
}

/**
 * @brief Periodic UI update (LCD refresh timing).
 *
 * @details
 * This function does not draw anything itself. It simply determines when
 * the LCD should be refreshed. The actual rendering is performed by uiLoop().
 */
void uiStateUpdate() {
    uint32_t now = millis();

    if (now - g_uiState.lastRefreshMs >= UI_REFRESH_INTERVAL_MS) {
        g_uiState.lastRefreshMs = now;
        // Rendering happens in uiLoop() (ui.cpp)
    }
}