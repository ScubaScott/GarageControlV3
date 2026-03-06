/**
 * @file menu_callbacks.h
 * @brief Declarations for menu action handlers.
 *
 * @ingroup UI
 */

#pragma once

/** @brief Toggle lights manually. */
void menuToggleLights();

/** @brief Trigger a door pulse. */
void menuPulseDoor();

/** @brief Cycle heater mode (OFF → AUTO → MANUAL). */
void menuHeaterMode();

/** @brief Begin editing the "heat on" temperature threshold. */
void menuSetHeatOn();

/** @brief Begin editing the "heat off" temperature threshold. */
void menuSetHeatOff();

/** @brief Exit the menu and return to the status screen. */
void menuExit();