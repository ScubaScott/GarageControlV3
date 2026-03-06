/**
 * @file actuators.h
 * @brief Actuator subsystem interface for the Smart Garage Controller.
 *
 * @details
 * This subsystem provides a hardware abstraction layer for all physical outputs:
 * - Lights relay
 * - Heater relay
 * - Door button relay (momentary pulse)
 *
 * It exposes:
 * - Relay configuration structures
 * - ActuatorState (desired output state)
 * - Initialization and update functions
 * - A timestamp accessor used by the Sensors subsystem to suppress PIR noise
 *
 * @note
 * The heater relay is known to generate electrical noise when switching.
 * To prevent false PIR motion triggers, the Actuators subsystem records the
 * timestamp of every heater relay transition (ON or OFF). The Sensors subsystem
 * uses this timestamp to ignore PIR rising edges for a short suppression window.
 *
 * @defgroup Actuators Actuators Subsystem
 * @{
 */

#pragma once
#include <stdint.h>

/**
 * @brief Configuration for a single relay output.
 */
struct RelayConfig {
    uint8_t pin;        /**< Arduino digital pin used for this relay */
    bool    activeHigh; /**< True if relay is active-high, false if active-low */
};

/**
 * @brief Configuration for all actuators.
 */
struct ActuatorConfig {
    RelayConfig lights;       /**< Lights relay configuration */
    RelayConfig heater;       /**< Heater relay configuration */
    RelayConfig door;         /**< Door button relay configuration */

    uint16_t doorPulseMs;     /**< Duration of door pulse in milliseconds */
    uint16_t doorCooldownMs;  /**< Minimum time between door pulses in ms */
};

/**
 * @brief Desired actuator state as produced by the Control Logic subsystem.
 */
struct ActuatorState {
    bool lightsOn;         /**< True to energize lights relay */
    bool heaterOn;         /**< True to energize heater relay */
    bool doorPulseRequest; /**< True to request a door button pulse */
};

/**
 * @brief Initializes actuator pins and internal timing state.
 * @param cfg Actuator configuration.
 */
void actuatorsInit(const ActuatorConfig& cfg);

/**
 * @brief Applies the desired actuator state to the physical outputs.
 *
 * @details
 * - Drives lights and heater relays directly.
 * - Generates a timed pulse for the door relay when requested.
 * - Tracks heater relay transitions and records timestamps for PIR suppression.
 *
 * @param state Desired actuator state.
 */
void actuatorsApply(const ActuatorState& state);

/**
 * @brief Returns the timestamp of the last heater relay transition.
 *
 * @details
 * The Sensors subsystem uses this timestamp to suppress PIR rising edges
 * for a short period after the heater relay toggles, preventing false
 * motion events caused by electrical noise.
 *
 * @return Milliseconds since boot when the heater relay last changed state.
 */
unsigned long actuatorsLastHeaterChangeMs();

/** @} */ // end of Actuators group