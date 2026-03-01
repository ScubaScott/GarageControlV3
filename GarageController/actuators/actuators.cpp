#include "actuators.h"
#include <Arduino.h>

static ActuatorConfig config;

static bool doorPulsing        = false;
static uint32_t doorPulseStart = 0;
static uint32_t doorCooldownStart = 0;

static void writeRelay(const RelayConfig& r, bool on) {
    digitalWrite(r.pin, on ? HIGH : LOW);  // active-high
}

void actuatorsInit(const ActuatorConfig& cfg) {
    config = cfg;

    pinMode(config.lights.pin, OUTPUT);
    pinMode(config.heater.pin, OUTPUT);
    pinMode(config.door.pin,   OUTPUT);

    writeRelay(config.lights, false);
    writeRelay(config.heater, false);
    writeRelay(config.door,   false);

    doorPulsing        = false;
    doorPulseStart     = 0;
    doorCooldownStart  = 0;
}

void actuatorsApply(const ActuatorState& desired) {
    uint32_t now = millis();

    // Lights + heater
    writeRelay(config.lights, desired.lightsOn);
    writeRelay(config.heater, desired.heaterOn);

    // Door pulse
    if (desired.doorPulseRequest &&
        !doorPulsing &&
        (now - doorCooldownStart >= config.doorCooldownMs)) {

        doorPulsing    = true;
        doorPulseStart = now;
        writeRelay(config.door, true);
    }

    if (doorPulsing && (now - doorPulseStart >= config.doorPulseMs)) {
        doorPulsing       = false;
        doorCooldownStart = now;
        writeRelay(config.door, false);
    }
}