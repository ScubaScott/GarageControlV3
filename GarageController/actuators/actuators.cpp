/**
 * @file actuators.cpp
 * @brief Implementation of the Actuators subsystem.
 *
 * @details
 * This module drives all physical outputs:
 * - Lights relay
 * - Heater relay
 * - Door button relay (timed pulse)
 *
 * It also tracks the timestamp of the most recent heater relay transition.
 * The Sensors subsystem uses this timestamp to suppress PIR rising edges
 * for a short period after heater relay switching, preventing false motion
 * events caused by electrical noise.
 *
 * @ingroup Actuators
 */

#include "actuators.h"
#include <Arduino.h>

// ---------------------------------------------------------------------------
// Internal State
// ---------------------------------------------------------------------------

/** @brief Cached actuator configuration. */
static ActuatorConfig g_cfg;

/** @brief Tracks whether the heater relay is currently energized. */
static bool g_heaterState = false;

/** @brief Timestamp of the last heater relay ON/OFF transition. */
static unsigned long g_lastHeaterChangeMs = 0;

/** @brief Tracks whether a door pulse is currently active. */
static bool g_doorPulseActive = false;

/** @brief Timestamp when the current door pulse began. */
static unsigned long g_doorPulseStartMs = 0;

/** @brief Timestamp of the last completed door pulse. */
static unsigned long g_lastDoorPulseMs = 0;

// ---------------------------------------------------------------------------
// Initialization
// ---------------------------------------------------------------------------

void actuatorsInit(const ActuatorConfig& cfg) {
    g_cfg = cfg;

    pinMode(g_cfg.lights.pin, OUTPUT);
    pinMode(g_cfg.heater.pin, OUTPUT);
    pinMode(g_cfg.door.pin, OUTPUT);

    // Initialize all relays to OFF
    digitalWrite(g_cfg.lights.pin, g_cfg.lights.activeHigh ? LOW : HIGH);
    digitalWrite(g_cfg.heater.pin, g_cfg.heater.activeHigh ? LOW : HIGH);
    digitalWrite(g_cfg.door.pin,   g_cfg.door.activeHigh   ? LOW : HIGH);

    g_heaterState = false;
    g_lastHeaterChangeMs = 0;
    g_doorPulseActive = false;
    g_doorPulseStartMs = 0;
    g_lastDoorPulseMs = 0;
}

// ---------------------------------------------------------------------------
// Heater Relay Handling
// ---------------------------------------------------------------------------

/**
 * @brief Drives the heater relay and records transition timestamps.
 *
 * @param desiredOn True to energize the heater relay.
 */
static void applyHeaterRelay(bool desiredOn) {
    if (desiredOn != g_heaterState) {
        // Relay state changed — record timestamp for PIR suppression
        g_lastHeaterChangeMs = millis();
        g_heaterState = desiredOn;

        digitalWrite(
            g_cfg.heater.pin,
            (g_cfg.heater.activeHigh ? desiredOn : !desiredOn) ? HIGH : LOW
        );
    }
}

// ---------------------------------------------------------------------------
// Lights Relay Handling
// ---------------------------------------------------------------------------

/**
 * @brief Drives the lights relay.
 *
 * @param on True to energize the lights relay.
 */
static void applyLightsRelay(bool on) {
    digitalWrite(
        g_cfg.lights.pin,
        (g_cfg.lights.activeHigh ? on : !on) ? HIGH : LOW
    );
}

// ---------------------------------------------------------------------------
// Door Relay Handling
// ---------------------------------------------------------------------------

/**
 * @brief Handles the door button relay pulse timing.
 *
 * @param requestPulse True if a new pulse is requested.
 */
static void applyDoorRelay(bool requestPulse) {
    unsigned long now = millis();

    // Start a new pulse if requested and cooldown has elapsed
    if (requestPulse && !g_doorPulseActive &&
        (now - g_lastDoorPulseMs >= g_cfg.doorCooldownMs)) {

        g_doorPulseActive = true;
        g_doorPulseStartMs = now;

        digitalWrite(
            g_cfg.door.pin,
            g_cfg.door.activeHigh ? HIGH : LOW
        );
    }

    // End the pulse if active and duration exceeded
    if (g_doorPulseActive &&
        (now - g_doorPulseStartMs >= g_cfg.doorPulseMs)) {

        g_doorPulseActive = false;
        g_lastDoorPulseMs = now;

        digitalWrite(
            g_cfg.door.pin,
            g_cfg.door.activeHigh ? LOW : HIGH
        );
    }
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void actuatorsApply(const ActuatorState& state) {
    applyLightsRelay(state.lightsOn);
    applyHeaterRelay(state.heaterOn);
    applyDoorRelay(state.doorPulseRequest);
}

unsigned long actuatorsLastHeaterChangeMs() {
    return g_lastHeaterChangeMs;
}