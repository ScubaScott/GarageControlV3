#include <Arduino.h>
#include <Wire.h>
#include "ui/ui.h"
#include "sensors/sensors.h"
#include "actuators/actuators.h"
#include "control/control.h"

// -----------------------------
// Global configuration
// -----------------------------

SensorConfig sensorCfg = {
    .reedOpenActiveHigh   = true,
    .reedClosedActiveHigh = true,
    .debounceMs           = 25
};

ActuatorConfig actuatorCfg = {
    .lights = { .pin = 8,  .activeHigh = true },
    .heater = { .pin = 9,  .activeHigh = true },
    .door   = { .pin = 10, .activeHigh = true },
    .doorPulseMs    = 1000,
    .doorCooldownMs = 3000
};

SystemConfig systemCfg = {
    .heatOnBelowC            = 7,        // ~45°F
    .heatOffAboveC           = 10,       // ~50°F
    .lightsTimeoutMs         = 5UL * 60UL * 1000UL,
    .doorAutoCloseTimeoutMs  = 10UL * 60UL * 1000UL,
    .doorTravelTimeMs        = 18000,
    .heatDoorGraceMs         = 3UL * 60UL * 1000UL,
    .heatMode                = HEAT_AUTO
};

// -----------------------------
// Setup
// -----------------------------

void setup() {
    Serial.begin(115200);
    delay(200);

    Wire.begin();

    sensorsInit(sensorCfg);
    actuatorsInit(actuatorCfg);
    controlInit(systemCfg);
    uiInit();

    Serial.println("Garage Controller Initialized");
}

// -----------------------------
// Main Loop
// -----------------------------

void loop() {
    // 1. UI input
    ButtonEvent e = pollButtons();
    if (e != BTN_NONE) {
        uiHandleEvent(e);
    }

    // 2. Sensors
    sensorsPoll();
    SensorState sensors = sensorsGetState();

    // 3. Control logic
    controlUpdate(sensors);
    ActuatorState act = controlGetActuators();

    // 4. Apply actuators
    actuatorsApply(act);

    // 5. UI update
    uiLoop();
}