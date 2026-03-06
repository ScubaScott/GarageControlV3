/**
 * @file GarageController.ino
 * @brief Main application entry point for the Smart Garage Controller.
 *
 * @details
 * This file orchestrates the entire system by coordinating all subsystems:
 *
 * - Sensors subsystem:
 *      - Reads PIR, reed switches, DS18B20 temperature
 * - Control Logic subsystem:
 *      - Heater hysteresis, lighting automation, door grace timing
 * - Actuators subsystem:
 *      - Drives relays for lights, heater, and door pulse
 * - UI subsystem:
 *      - Button input, menu navigation, LCD rendering
 *
 * The main loop performs the following sequence:
 * 1. Poll buttons
 * 2. Handle UI events
 * 3. Poll sensors
 * 4. Run control logic
 * 5. Apply actuator outputs
 * 6. Update UI rendering
 *
 * @ingroup MainApp
 */

#include <Arduino.h>
#include "ui/ui.h"
#include "sensors.h"
#include "actuators.h"
#include "control.h"

// ---------------------------------------------------------------------------
// Sensor Configuration
// ---------------------------------------------------------------------------

/**
 * @brief Sensor configuration for the Smart Garage Controller.
 *
 * @details
 * - Reed switches use internal pull-ups
 * - Debounce time is 25 ms
 * - DS18B20 is connected to pin 11
 */
SensorConfig sensorCfg = {
    .reedOpenActiveHigh   = true,
    .reedClosedActiveHigh = true,
    .debounceMs           = 25,
    .ds18b20Pin           = 11
};

// ---------------------------------------------------------------------------
// Actuator Configuration
// ---------------------------------------------------------------------------

/**
 * @brief Relay and timing configuration for actuators.
 *
 * @details
 * - Lights relay on pin 8
 * - Heater relay on pin 9
 * - Door button relay on pin 10
 * - Door pulse lasts 1000 ms
 * - Minimum 3000 ms between pulses
 */
ActuatorConfig actuatorCfg = {
    .lights = { .pin = 8,  .activeHigh = true },
    .heater = { .pin = 9,  .activeHigh = true },
    .door   = { .pin = 10, .activeHigh = true },
    .doorPulseMs    = 1000,
    .doorCooldownMs = 3000
};

// ---------------------------------------------------------------------------
// System Configuration
// ---------------------------------------------------------------------------

/**
 * @brief System-wide behavior configuration.
 *
 * @details
 * - Heater turns ON below 7°C
 * - Heater turns OFF above 10°C
 * - Lights auto-off after 5 minutes
 * - Door grace period for heater: 3 minutes
 * - Door travel time: 18 seconds
 * - Heater mode: AUTO
 */
SystemConfig systemCfg = {
    .heatOnBelowC            = 7,
    .heatOffAboveC           = 10,
    .lightsTimeoutMs         = 5UL * 60UL * 1000UL,
    .doorAutoCloseTimeoutMs  = 10UL * 60UL * 1000UL,
    .doorTravelTimeMs        = 18000,
    .heatDoorGraceMs         = 3UL * 60UL * 1000UL,
    .heatMode                = HEAT_AUTO
};

// ---------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------

/**
 * @brief Arduino setup routine.
 *
 * @details
 * Initializes:
 * - Serial port
 * - Sensors subsystem
 * - Actuators subsystem
 * - Control logic subsystem
 * - UI subsystem
 */
void setup() {
    Serial.begin(115200);
    delay(200);

    sensorsInit(sensorCfg);
    actuatorsInit(actuatorCfg);
    controlInit(systemCfg);
    uiInit();

    Serial.println("Smart Garage Controller Initialized");
}

// ---------------------------------------------------------------------------
// Main Loop
// ---------------------------------------------------------------------------

/**
 * @brief Main application loop.
 *
 * @details
 * Executes the full system cycle:
 * 1. Poll buttons
 * 2. Handle UI events
 * 3. Poll sensors
 * 4. Run control logic
 * 5. Apply actuator outputs
 * 6. Update UI rendering
 */
void loop() {
    // 1. Poll buttons
    ButtonEvent ev = pollButtons();

    // 2. Feed event into UI state machine
    if (ev != BTN_NONE) {
        uiHandleEvent(ev);
    }

    // 3. Poll sensors
    sensorsPoll();
    SensorState sensors = sensorsGetState();

    // 4. Run control logic
    controlUpdate(sensors);
    ActuatorState act = controlGetActuators();

    // 5. Apply actuator outputs
    actuatorsApply(act);

    // 6. Update UI rendering
    uiLoop();
}