/**
 * @file ui_state.h
 * @brief UI state machine definitions for the Smart Garage Controller.
 *
 * @details
 * The UI state machine tracks:
 * - The current screen (status, menus, editing screens)
 * - Menu navigation state
 * - Editing modes for configuration values
 * - LCD refresh timing
 *
 * This file defines the structures and enums used internally by the UI
 * subsystem. The main application interacts with the UI only through:
 * - uiInit()
 * - pollButtons()
 * - uiHandleEvent()
 * - uiLoop()
 *
 * @ingroup UI
 */

#pragma once
#include <stdint.h>

/**
 * @brief High-level UI screens.
 *
 * @details
 * The UI can be in one of several modes:
 * - STATUS: Main status display (temperature, door, heater, lights)
 * - MENU:   Menu navigation mode
 * - EDIT:   Editing a configuration value
 */
enum UIScreen : uint8_t {
    UI_STATUS = 0,   /**< Main status screen */
    UI_MENU,         /**< Menu navigation screen */
    UI_EDIT          /**< Editing a configuration value */
};

/**
 * @brief Represents a single menu entry.
 *
 * @details
 * Each menu entry has:
 * - A display label
 * - A callback function invoked when SELECT is pressed
 */
struct MenuItem {
    const char* label;                 /**< Text label shown on LCD */
    void (*onSelect)();                /**< Callback when SELECT is pressed */
};

/**
 * @brief Represents the current UI state.
 *
 * @details
 * Tracks:
 * - Current screen (status/menu/edit)
 * - Current menu index
 * - LCD refresh timing
 * - Editing state (if applicable)
 */
struct UIState {
    UIScreen screen;                   /**< Current UI screen */
    uint8_t menuIndex;                 /**< Current menu selection index */

    bool editing;                      /**< True if editing a value */
    uint32_t lastRefreshMs;            /**< Timestamp of last LCD refresh */
};

/**
 * @brief Global UI state instance (owned by ui_state.cpp).
 */
extern UIState g_uiState;

/**
 * @brief Initializes the UI state machine.
 *
 * @details
 * Called once from uiInit().
 */
void uiStateInit();

/**
 * @brief Advances the UI state machine based on a button event.
 *
 * @param ev Button event (UP/DOWN/SELECT).
 */
void uiStateHandleEvent(uint8_t ev);

/**
 * @brief Performs periodic UI updates (LCD refresh, animations, etc.).
 *
 * @details
 * Called once per loop() from uiLoop().
 */
void uiStateUpdate();
