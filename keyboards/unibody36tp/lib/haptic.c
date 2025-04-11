#include QMK_KEYBOARD_H
#include "haptic.h"
#include "drv2605l.h"

bool haptic_module_enabled = true;
uint16_t haptic_module_pattern = DRV2605L_EFFECT_SHARP_TICK_1_100;

void haptic_module_pulse_default(void) {
    if (haptic_module_enabled) drv2605l_pulse(haptic_module_pattern);
}

void haptic_module_small_tick(void) {
    if (haptic_module_enabled) drv2605l_pulse(DRV2605L_EFFECT_SHARP_TICK_3_60);
}

void haptic_module_double_tick(void) {
    if (haptic_module_enabled) drv2605l_pulse(DRV2605L_EFFECT_SHORT_DOUBLE_SHARP_TICK_3_60);
}


uint32_t haptic_module_pulse_default_callback(uint32_t trigger_time, void *cb_arg) {
    haptic_module_pulse_default();
    return 0; 
}

void haptic_module_pulse_defer(uint16_t delay) {
    if (haptic_module_enabled) defer_exec(delay, haptic_module_pulse_default_callback, NULL);
}

bool haptic_module_toggle(void) {
    haptic_module_enabled = !haptic_module_enabled;
    if (haptic_module_enabled) {
        haptic_module_pattern = DRV2605L_EFFECT_SHARP_TICK_1_100;
        haptic_module_pulse_default();
    }
    return haptic_module_enabled;
}

void haptic_module_increase(void) {
    if (haptic_module_enabled) { 
        haptic_module_pattern = (haptic_module_pattern < DRV2605L_EFFECT_SMOOTH_HUM_5_10) ? haptic_module_pattern + 1 : DRV2605L_EFFECT_SMOOTH_HUM_5_10;
        haptic_module_pulse_default();
    }
}

void haptic_module_decrease(void) {
    if (haptic_module_enabled) {
        haptic_module_pattern = (haptic_module_pattern > DRV2605L_EFFECT_STRONG_CLICK_100) ? haptic_module_pattern - 1 : DRV2605L_EFFECT_STRONG_CLICK_100;
        haptic_module_pulse_default();
    }
}
