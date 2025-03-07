static uint16_t trackpoint_timer;
extern int tp_buttons; // mousekey button state set in action.c and used in ps2_mouse.c

void trackpoint_reset_ps2(void) {
    gpio_set_pin_output(PS2RESETPIN);
    gpio_write_pin(PS2RESETPIN,true);
    wait_ms(2);
    gpio_write_pin(PS2RESETPIN,false);
}
