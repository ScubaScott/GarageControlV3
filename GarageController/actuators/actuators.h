#pragma once
#include <stdint.h>

struct ActuatorState {
    bool lightsOn;
    bool heaterOn;
    bool doorPulseRequest;   // one-shot request
};

struct RelayConfig {
    uint8_t pin;
    bool activeHigh;         // true for your setup
};

struct ActuatorConfig {
    RelayConfig lights;
    RelayConfig heater;
    RelayConfig door;
    uint16_t doorPulseMs;      // e.g. 1000
    uint16_t doorCooldownMs;   // e.g. 3000
};

void actuatorsInit(const ActuatorConfig& cfg);
void actuatorsApply(const ActuatorState& desired);