/**
 * @file control.cpp
 * @brief Implementation of the Control Logic subsystem.
 *
 * @details
 * This module implements:
 * - Heater hysteresis (AUTO mode)
 * - Lighting automation based on PIR motion
 * - Door grace timing for heater suppression
 * - Door auto-close timing
 *
 * @section PIR_Interaction PIR Interaction Notes
 * The Control subsystem does not filter PIR input. It trusts the
 * Sensors subsystem to provide a clean motion signal. The Sensors
 * subsystem applies a suppression window after heater relay transitions
 * to prevent false motion events caused by electrical noise.
 *
 * Control logic behavior:
 * - If motion == true → lights turn on immediately
 * - If motion == false → lights may time out
 * - Heater logic is unaffected by PIR suppression
 *
 * @ingroup Control
 */

#include "control.h"
#include <Arduino.h>

// ---------------------------------------------------------------------------
// Internal State
// ---------------------------------------------------------------------------

static SystemConfig g_cfg;
static ActuatorState g_act;

static unsigned long g_lastMotionMs = 0;
static unsigned long g_doorMoveStartMs = 0;

// ---------------------------------------------------------------------------
// Initialization
// ---------------------------------------------------------------------------

void controlInit(const SystemConfig& cfg) {
    g_cfg = cfg;

    g_act.lightsOn = false;
    g_act.heaterOn = false;
    g_act.doorPulseRequest = false;

    g_lastMotionMs = 0;
    g_doorMoveStartMs = 0;
}

// ---------------------------------------------------------------------------
// Update Logic
// ---------------------------------------------------------------------------

void controlUpdate(const SensorState& s) {
    unsigned long now = millis();

    // -----------------------------
    // Motion-based lighting
    // -----------------------------
    if (s.motion) {
        g_lastMotionMs = now;
        g_act.lightsOn = true;
    } else {
        if (now - g_lastMotionMs >= g_cfg.lightsTimeoutMs) {
            g_act.lightsOn = false;
        }
    }

    // -----------------------------
    // Heater hysteresis
    // -----------------------------
    if (g_cfg.heatMode == 0 /* AUTO */) {
        if (s.temperatureValid) {
            if (s.temperatureC <= g_cfg.heatOnBelowC) {
                g_act.heaterOn = true;
            } else if (s.temperatureC >= g_cfg.heatOffAboveC) {
                g_act.heaterOn = false;
            }
        }
    }

    // -----------------------------
    // Door auto-close logic
    // -----------------------------
    if (s.doorOpen && !s.doorClosed) {
        if (g_doorMoveStartMs == 0) {
            g_doorMoveStartMs = now;
        }

        if (now - g_doorMoveStartMs >= g_cfg.doorAutoCloseTimeoutMs) {
            g_act.doorPulseRequest = true;
            g_doorMoveStartMs = now;
        }
    } else {
        g_doorMoveStartMs = 0;
    }
}

// ---------------------------------------------------------------------------
// Accessor
// ---------------------------------------------------------------------------

ActuatorState controlGetActuators() {
    return g_act;
}