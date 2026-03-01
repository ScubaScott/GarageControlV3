#include "menu_callbacks.h"
#include "ui_state.h"
#include "control.h"
#include "actions.h"

extern UiContext ui;

// -------------------------
// Helper: enter edit mode
// -------------------------
static void startEdit(const char* label, int* value, int minV, int maxV, int step) {
    ui.edit.label = label;
    ui.edit.valuePtr = value;
    ui.edit.minValue = minV;
    ui.edit.maxValue = maxV;
    ui.edit.step = step;
    ui.state = UI_EDIT_VALUE;
    ui.lcdNeedsUpdate = true;
}

// -------------------------
// Helper: enter confirm mode
// -------------------------
static void startConfirm(const char* prompt, void (*fn)()) {
    ui.confirm.prompt = prompt;
    ui.confirm.onConfirm = fn;
    ui.state = UI_CONFIRM;
    ui.lcdNeedsUpdate = true;
}

// -------------------------
// HEAT SETTINGS
// -------------------------

void cbHeatOnBelow() {
    SystemConfig cfg = controlGetConfig();
    startEdit("Heat On Below", &cfg.heatOnBelowC, -20, 40, 1);
    controlSetConfig(cfg);
}

void cbHeatOffAbove() {
    SystemConfig cfg = controlGetConfig();
    startEdit("Heat Off Above", &cfg.heatOffAboveC, -10, 50, 1);
    controlSetConfig(cfg);
}

void cbHeatMode() {
    SystemConfig cfg = controlGetConfig();
    cfg.heatMode = (HeatMode)((cfg.heatMode + 1) % 3);  // cycle OFF → AUTO → MANUAL
    controlSetConfig(cfg);
    ui.lcdNeedsUpdate = true;
}

void cbHeatDoorGrace() {
    SystemConfig cfg = controlGetConfig();
    int minutes = cfg.heatDoorGraceMs / 60000;
    startEdit("Door Grace (min)", &minutes, 0, 30, 1);
    cfg.heatDoorGraceMs = minutes * 60000UL;
    controlSetConfig(cfg);
}

// -------------------------
// LIGHT SETTINGS
// -------------------------

void cbLightTimeout() {
    SystemConfig cfg = controlGetConfig();
    int minutes = cfg.lightsTimeoutMs / 60000;
    startEdit("Light Timeout", &minutes, 1, 60, 1);
    cfg.lightsTimeoutMs = minutes * 60000UL;
    controlSetConfig(cfg);
}

void cbManualLights() {
    controlToggleLightsManual();
    ui.lcdNeedsUpdate = true;
}

// -------------------------
// DOOR SETTINGS
// -------------------------

void cbDoorAutoClose() {
    SystemConfig cfg = controlGetConfig();
    int minutes = cfg.doorAutoCloseTimeoutMs / 60000;
    startEdit("Auto Close (min)", &minutes, 0, 60, 1);
    cfg.doorAutoCloseTimeoutMs = minutes * 60000UL;
    controlSetConfig(cfg);
}

void cbDoorTravelTime() {
    SystemConfig cfg = controlGetConfig();
    int seconds = cfg.doorTravelTimeMs / 1000;
    startEdit("Travel Time (sec)", &seconds, 5, 60, 1);
    cfg.doorTravelTimeMs = seconds * 1000UL;
    controlSetConfig(cfg);
}

static void doManualDoorPulse() {
    controlRequestDoorPulse();
}

void cbManualDoorClose() {
    startConfirm("Pulse Door Relay?", doManualDoorPulse);
}

// -------------------------
// SYSTEM SETTINGS
// -------------------------

void cbUnits() {
    // TODO: implement °F/°C toggle
}

void cbLcdBrightness() {
    // TODO: implement brightness edit
}

void cbLcdTimeout() {
    // TODO: implement LCD timeout edit
}

static void doReboot() {
    NVIC_SystemReset();  // Uno R4 reset
}

void cbReboot() {
    startConfirm("Reboot Controller?", doReboot);
}