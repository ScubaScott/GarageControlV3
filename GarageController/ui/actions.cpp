#include "actions.h"
#include "control.h"

void pulseDoorRelay() {
    controlRequestDoorPulse();
}

void toggleLights() {
    controlToggleLightsManual();
}


void saveConfig() {
    // TODO: write to NVS
}