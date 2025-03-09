#include QMK_KEYBOARD_H
#include "trackpoint.h"

void trackpoint_reset_ps2(void) {
    gpio_set_pin_output(PS2RESETPIN);
    gpio_write_pin(PS2RESETPIN,true);
    wait_ms(2);
    gpio_write_pin(PS2RESETPIN,false);
}