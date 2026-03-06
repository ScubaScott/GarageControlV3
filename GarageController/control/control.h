/**
 * @file control.h
 * @brief Control Logic subsystem interface.
 *
 * @details
 * This subsystem consumes SensorState and produces ActuatorState.
 * It implements:
 * - Heater hysteresis control
 * - Lighting automation based on PIR motion
 * - Door grace timing for heater suppression
 * - Door auto-close timing
 *
 * @section PIR_Interaction PIR Interaction Notes
 * The Control subsystem does not perform PIR filtering or suppression.
 * Instead, it relies on the Sensors subsystem to provide a clean,
 * noise-free motion signal. The Sensors subsystem applies a suppression
 * window after heater relay transitions to prevent false motion events.
 *
 * Control logic simply reacts to:
 * - motion == true → lights on immediately
 * - motion == false → lights may time out
 *
 * @ingroup Control
 */

#pragma once
#include "sensors.h"
#include "actuators.h"

/**
 * @brief System-wide behavior configuration.
 */
struct SystemConfig {
    int heatOnBelowC;             /**< Heater turns ON below this temperature */
    int heatOffAboveC;            /**< Heater turns OFF above this temperature */

    unsigned long lightsTimeoutMs;        /**< Lights auto-off timeout */
    unsigned long doorAutoCloseTimeoutMs; /**< Auto-close timeout for garage door */
    unsigned long doorTravelTimeMs;       /**< Time for door to fully open/close */
    unsigned long heatDoorGraceMs;        /**< Heater suppression after door movement */

    uint8_t heatMode;             /**< Manual/AUTO heater mode */
};

/**
 * @brief Initializes control logic.
 * @param cfg System configuration.
 */
void controlInit(const SystemConfig& cfg);

/**
 * @brief Updates control logic based on current sensor readings.
 * @param sensors Current sensor state.
 */
void controlUpdate(const SensorState& sensors);

/**
 * @brief Returns the most recently computed actuator state.
 * @return ActuatorState structure.
 */
ActuatorState controlGetActuators();