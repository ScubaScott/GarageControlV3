#pragma once
#include <stdint.h>

struct SensorState {
    float temperatureC;
    bool temperatureValid;

    bool motion;

    bool doorOpen;
    bool doorClosed;

    bool doorError;
};

struct SensorConfig {
    bool reedOpenActiveHigh;     // default true
    bool reedClosedActiveHigh;   // default true
    uint16_t debounceMs;
};

void sensorsInit(const SensorConfig& cfg);
void sensorsPoll();                 // call every loop
SensorState sensorsGetState();      // latest stable values