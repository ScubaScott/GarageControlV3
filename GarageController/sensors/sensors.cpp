#include "sensors.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>

static SensorConfig config;
static SensorState state;

static const uint8_t PIN_PIR         = 5;
static const uint8_t PIN_REED_OPEN   = 6;
static const uint8_t PIN_REED_CLOSED = 7;

static Adafruit_BME280 bme;
static bool bmeOk = false;

struct Debounce {
    bool stable;
    bool lastRaw;
    uint32_t lastChangeMs;
};

static Debounce dbOpen;
static Debounce dbClosed;

static bool applyPolarity(bool raw, bool activeHigh) {
    return activeHigh ? raw : !raw;
}

static void updateDebounce(Debounce& d, bool raw, uint32_t now) {
    if (raw != d.lastRaw) {
        d.lastRaw = raw;
        d.lastChangeMs = now;
    }
    if ((now - d.lastChangeMs) >= config.debounceMs) {
        d.stable = d.lastRaw;
    }
}

void sensorsInit(const SensorConfig& cfg) {
    config = cfg;

    pinMode(PIN_PIR, INPUT);
    pinMode(PIN_REED_OPEN, INPUT_PULLUP);
    pinMode(PIN_REED_CLOSED, INPUT_PULLUP);

    dbOpen   = {false, digitalRead(PIN_REED_OPEN), millis()};
    dbClosed = {false, digitalRead(PIN_REED_CLOSED), millis()};

    Wire.begin();
    bmeOk = bme.begin(0x76);   // adjust if needed

    state.temperatureC     = 0.0f;
    state.temperatureValid = false;
    state.motion           = false;
    state.doorOpen         = false;
    state.doorClosed       = false;
    state.doorError        = false;
}

void sensorsPoll() {
    uint32_t now = millis();

    // PIR
    state.motion = (digitalRead(PIN_PIR) == HIGH);

    // Reed switches (debounced + polarity)
    bool rawOpen   = digitalRead(PIN_REED_OPEN);
    bool rawClosed = digitalRead(PIN_REED_CLOSED);

    updateDebounce(dbOpen, rawOpen, now);
    updateDebounce(dbClosed, rawClosed, now);

    state.doorOpen   = applyPolarity(dbOpen.stable,   config.reedOpenActiveHigh);
    state.doorClosed = applyPolarity(dbClosed.stable, config.reedClosedActiveHigh);

    state.doorError = (state.doorOpen && state.doorClosed);

    // Temperature
    if (bmeOk) {
        float t = bme.readTemperature();
        if (!isnan(t)) {
            state.temperatureC     = t;
            state.temperatureValid = true;
        } else {
            state.temperatureValid = false;
        }
    } else {
        state.temperatureValid = false;
    }
}

SensorState sensorsGetState() {
    return state;
}