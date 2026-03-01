#pragma once
#include <stdint.h>
#include "../sensors/sensors.h"
#include "../actuators/actuators.h"

enum HeatMode {
    HEAT_OFF = 0,
    HEAT_AUTO = 1,
    HEAT_MANUAL = 2
};

struct SystemConfig {
    int heatOnBelowC;
    int heatOffAboveC;
    unsigned long lightsTimeoutMs;
    unsigned long doorAutoCloseTimeoutMs;
    unsigned long doorTravelTimeMs;
    unsigned long heatDoorGraceMs;   // new
    HeatMode heatMode;               // OFF / AUTO / MANUAL
};

void controlInit(const SystemConfig& cfg);
void controlUpdate(const SensorState& sensors);   // call every loop
ActuatorState controlGetActuators();
SystemConfig controlGetConfig();
void controlSetConfig(const SystemConfig& cfg);

// UI helpers
void controlRequestDoorPulse();
void controlToggleLightsManual();