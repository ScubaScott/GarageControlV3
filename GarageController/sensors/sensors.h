/**
 * @file sensors.h
 * @brief Sensor subsystem interface for the Smart Garage Controller.
 *
 * @details
 * This subsystem reads all physical inputs:
 * - PIR motion sensor
 * - Door open/closed reed switches
 * - DS18B20 temperature sensor
 *
 * It performs debouncing, state tracking, and validity checks.
 *
 * @section PIR_Suppression PIR Noise Suppression
 * The heater relay is known to generate electrical noise when switching ON or OFF.
 * To prevent false PIR motion triggers, the Sensors subsystem queries the
 * Actuators subsystem for the timestamp of the most recent heater relay transition.
 *
 * If a PIR rising edge occurs within a suppression window after a heater relay
 * transition, the rising edge is ignored. However:
 * - If PIR was already HIGH before the heater relay toggled, it remains HIGH.
 * - Only NEW rising edges are suppressed.
 * - Door relay and lights relay do NOT trigger suppression.
 *
 * This ensures:
 * - Real motion always turns lights on immediately.
 * - Heater relay noise never causes false motion events.
 *
 * @ingroup Sensors
 */

#pragma once
#include <stdint.h>

/**
 * @brief Configuration for the Sensors subsystem.
 */
struct SensorConfig {
    bool reedOpenActiveHigh;     /**< True if the "door open" reed switch is active-high */
    bool reedClosedActiveHigh;   /**< True if the "door closed" reed switch is active-high */
    uint16_t debounceMs;         /**< Debounce time for reed switches */
    uint8_t ds18b20Pin;          /**< Digital pin for DS18B20 temperature sensor */
};

/**
 * @brief Current sensor readings and interpreted state.
 */
struct SensorState {
    bool doorOpen;        /**< True if the "open" reed switch is active */
    bool doorClosed;      /**< True if the "closed" reed switch is active */
    bool doorError;       /**< True if both or neither reed switches are active */

    bool motion;          /**< True if PIR motion is detected (after suppression logic) */

    float temperatureC;   /**< Temperature in Celsius */
    bool temperatureValid;/**< True if temperature reading is valid */
};

/**
 * @brief Initializes all sensors and internal state.
 * @param cfg Sensor configuration.
 */
void sensorsInit(const SensorConfig& cfg);

/**
 * @brief Polls all sensors and updates internal state.
 *
 * @details
 * - Reads PIR motion sensor.
 * - Applies PIR suppression logic based on heater relay transitions.
 * - Reads and debounces reed switches.
 * - Reads DS18B20 temperature sensor.
 */
void sensorsPoll();

/**
 * @brief Returns the most recently computed sensor state.
 * @return Current SensorState structure.
 */
SensorState sensorsGetState();