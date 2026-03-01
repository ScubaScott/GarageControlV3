#include "control.h"
#include <Arduino.h>

static SystemConfig cfg;
static ActuatorState act;

static unsigned long lastMotionMs      = 0;
static bool manualLightsOverride       = false;

static unsigned long doorOpenedAtMs    = 0;
static bool doorWasClosedLast          = true;

static void updateDoorTiming(const SensorState& s, unsigned long now) {
    bool isClosed = s.doorClosed && !s.doorError;
    bool isOpen   = s.doorOpen   && !s.doorError;

    if (doorWasClosedLast && isOpen) {
        doorOpenedAtMs = now;
    }
    doorWasClosedLast = isClosed;
}

static bool heaterAllowed(const SensorState& s, unsigned long now) {
    if (!s.temperatureValid) return false;
    if (s.doorError)         return false;

    switch (cfg.heatMode) {
        case HEAT_OFF:
            return false;

        case HEAT_MANUAL:
            return true;

        case HEAT_AUTO: {
            bool isClosed = s.doorClosed && !s.doorError;
            bool isOpen   = s.doorOpen   && !s.doorError;

            if (isClosed) return true;

            if (isOpen) {
                if (now - doorOpenedAtMs < cfg.heatDoorGraceMs) {
                    return true;
                } else {
                    return false;
                }
            }

            // MOVING: treat as open but within grace if recently opened
            if (now - doorOpenedAtMs < cfg.heatDoorGraceMs) {
                return true;
            }
            return false;
        }
    }
    return false;
}

static void updateHeater(const SensorState& s, unsigned long now) {
    bool allowed = heaterAllowed(s, now);

    if (!allowed) {
        act.heaterOn = false;
        return;
    }

    if (s.temperatureC <= cfg.heatOnBelowC) {
        act.heaterOn = true;
    } else if (s.temperatureC >= cfg.heatOffAboveC) {
        act.heaterOn = false;
    }
}

static void updateLights(const SensorState& s, unsigned long now) {
    if (s.motion) {
        lastMotionMs = now;
        if (!manualLightsOverride) {
            act.lightsOn = true;
        }
    }

    if (!manualLightsOverride) {
        if (now - lastMotionMs > cfg.lightsTimeoutMs) {
            act.lightsOn = false;
        }
    }
}

void controlInit(const SystemConfig& c) {
    cfg = c;

    act.lightsOn        = false;
    act.heaterOn        = false;
    act.doorPulseRequest = false;

    lastMotionMs        = millis();
    doorOpenedAtMs      = millis();
    doorWasClosedLast   = true;
    manualLightsOverride = false;
}

void controlUpdate(const SensorState& s) {
    unsigned long now = millis();

    updateDoorTiming(s, now);
    updateHeater(s, now);
    updateLights(s, now);

    // doorPulseRequest is set by UI helpers; actuators layer will consume it
}

ActuatorState controlGetActuators() {
    return act;
}

SystemConfig controlGetConfig() {
    return cfg;
}

void controlSetConfig(const SystemConfig& c) {
    cfg = c;
}

void controlRequestDoorPulse() {
    act.doorPulseRequest = true;
}

void controlToggleLightsManual() {
    manualLightsOverride = !manualLightsOverride;
    if (manualLightsOverride) {
        act.lightsOn = !act.lightsOn;
    }
}