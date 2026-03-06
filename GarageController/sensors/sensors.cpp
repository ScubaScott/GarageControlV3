/**
 * @file sensors.cpp
 * @brief Implementation of the Sensors subsystem.
 *
 * @details
 * This module implements all sensor reading logic for the Smart Garage Controller.
 * It includes:
 * - DS18B20 temperature acquisition
 * - PIR motion detection
 * - Reed switch debouncing and polarity correction
 *
 * The subsystem ensures stable, validated readings for use by the Control Logic.
 *
 * @ingroup Sensors
 */

#include "sensors.h"
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// ---------------------------------------------------------------------------
// Fixed pin assignments for non-configurable sensors
// ---------------------------------------------------------------------------
static const uint8_t PIN_PIR         = 5;  /**< PIR motion sensor input pin */
static const uint8_t PIN_REED_OPEN   = 6;  /**< Reed switch: door open */
static const uint8_t PIN_REED_CLOSED = 7;  /**< Reed switch: door closed */

// Global configuration and state
static SensorConfig config;
static SensorState state;

/**
 * @brief Internal structure for debouncing reed switches.
 */
struct Debounce {
    bool stable;            /**< Debounced stable value */
    bool lastRaw;           /**< Last raw reading */
    uint32_t lastChangeMs;  /**< Timestamp of last raw change */
};

static Debounce dbOpen;
static Debounce dbClosed;

// DS18B20 objects
static OneWire* oneWire = nullptr;
static DallasTemperature* ds = nullptr;

// ---------------------------------------------------------------------------
// Helper functions
// ---------------------------------------------------------------------------

/**
 * @brief Applies polarity correction for NO/NC reed switches.
 * @param raw Raw digital input value.
 * @param activeHigh True if the switch is active-high.
 * @return Corrected logical state.
 */
static bool applyPolarity(bool raw, bool activeHigh) {
    return activeHigh ? raw : !raw;
}

/**
 * @brief Updates a debounce structure based on raw input.
 * @param d Debounce structure to update.
 * @param raw Raw digital input.
 * @param now Current timestamp in ms.
 */
static void updateDebounce(Debounce& d, bool raw, uint32_t now) {
    if (raw != d.lastRaw) {
        d.lastRaw = raw;
        d.lastChangeMs = now;
    }
    if ((now - d.lastChangeMs) >= config.debounceMs) {
        d.stable = d.lastRaw;
    }
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void sensorsInit(const SensorConfig& cfg) {
    config = cfg;

    pinMode(PIN_PIR, INPUT);
    pinMode(PIN_REED_OPEN, INPUT_PULLUP);
    pinMode(PIN_REED_CLOSED, INPUT_PULLUP);

    dbOpen   = {false, digitalRead(PIN_REED_OPEN), millis()};
    dbClosed = {false, digitalRead(PIN_REED_CLOSED), millis()};

    oneWire = new OneWire(config.ds18b20Pin);
    ds = new DallasTemperature(oneWire);
    ds->begin();

    state = {0.0f, false, false, false, false, false};
}

void sensorsPoll() {
    uint32_t now = millis();

    // PIR motion
    state.motion = (digitalRead(PIN_PIR) == HIGH);

    // Reed switches
    bool rawOpen   = digitalRead(PIN_REED_OPEN);
    bool rawClosed = digitalRead(PIN_REED_CLOSED);

    updateDebounce(dbOpen, rawOpen, now);
    updateDebounce(dbClosed, rawClosed, now);

    state.doorOpen   = applyPolarity(dbOpen.stable,   config.reedOpenActiveHigh);
    state.doorClosed = applyPolarity(dbClosed.stable, config.reedClosedActiveHigh);

    state.doorError = (state.doorOpen && state.doorClosed);

    // DS18B20 temperature
    ds->requestTemperatures();
    float t = ds->getTempCByIndex(0);

    if (t > -100 && t < 150) {
        state.temperatureC = t;
        state.temperatureValid = true;
    } else {
        state.temperatureValid = false;
    }
}

SensorState sensorsGetState() {
    return state;
}