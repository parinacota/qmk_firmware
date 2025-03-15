#ifndef __LEDS_H
#define __LEDS_H

#include QMK_KEYBOARD_H

#define HS_AZURE       132, 102
#define HS_BLACK         0,   0
#define HS_BLUE        170, 255
#define HS_CHARTREUSE   64, 255
#define HS_CORAL        11, 176
#define HS_CYAN        128, 255
#define HS_GOLD         36, 255
#define HS_GOLDENROD    30, 218
#define HS_GREEN        85, 255
#define HS_MAGENTA     213, 255
#define HS_ORANGE       21, 255
#define HS_PINK        234, 128
#define HS_PURPLE      191, 255
#define HS_RED           0, 255
#define HS_SPRINGGREEN 106, 255
#define HS_TEAL        128, 255
#define HS_TURQUOISE   123,  90
#define HS_WHITE         0,   0
#define HS_YELLOW       43, 255
#define HS_OFF         HS_BLACK

#define LED_BOTTOM 0
#define LED_LEFT 1
#define LED_RIGHT 2
#define LED_COUNT 3

void leds_turn_off(void);
void leds_on_for_range(uint16_t delay, uint8_t hue, uint8_t sat, uint8_t start, uint8_t end);
void leds_on_for_at(uint16_t delay, uint8_t hue, uint8_t sat, uint8_t pos);
void leds_val_inc(void);
void leds_val_dec(void);
void leds_seths_at(uint8_t hue, uint8_t sat, uint8_t pos);
void leds_off_at(uint8_t pos);
void leds_seths_range(uint8_t hue, uint8_t sat, uint8_t start, uint8_t end);
void leds_off_range(uint8_t start, uint8_t end);
void leds_anim_start(uint8_t effect_id);
void leds_anim_stop(void);

#endif



