#ifndef HAPTIC_H
#define HAPTIC_H

#include QMK_KEYBOARD_H

void haptic_module_pulse_default(void);
void haptic_module_small_tick(void);
void haptic_module_double_tick(void);
void haptic_module_pulse_defer(uint16_t delay);
bool haptic_module_toggle(void);
void haptic_module_increase(void);
void haptic_module_decrease(void);

#endif
