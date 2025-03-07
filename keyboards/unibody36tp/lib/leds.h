#pragma once
#include <ws2812.h>

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

uint8_t _leds_val = 96;

void leds_turn_off(void) {
    rgblight_sethsv_range(HS_OFF, 0, 0, 3);
}

typedef struct  {
    uint8_t start;
    uint8_t end;
    deferred_token defer_token;
} leds_turn_off_callback_args;

uint32_t leds_turn_off_callback(uint32_t trigger_time, void *cb_arg) {
    leds_turn_off_callback_args *args = (leds_turn_off_callback_args *)cb_arg;
    rgblight_sethsv_range(HS_OFF, 0, args->start, args->end);
    args->defer_token = 0;
    return 0;
}

leds_turn_off_callback_args cb_args[3];

void leds_on_for_range(uint16_t delay, uint8_t hue, uint8_t sat, uint8_t start, uint8_t end) {    
    cb_args[start].start = start;
    cb_args[start].end = end;
    rgblight_sethsv_range(hue, sat, _leds_val, start, end);
    if (cb_args[start].defer_token) {
        cancel_deferred_exec(cb_args[start].defer_token);
    }
    cb_args[start].defer_token = defer_exec(delay, leds_turn_off_callback, (void*)&cb_args[start]);
}

void leds_val_inc(void) {
    if (_leds_val < RGBLIGHT_LIMIT_VAL-16) {
        _leds_val+=16;
    }
    leds_on_for_range(1000, HS_WHITE, 0, 3);
}

void leds_val_dec(void) {
    if (_leds_val > 16) {
        _leds_val-=16;
    }
    leds_on_for_range(1000, HS_WHITE, 0, 3);
}

void leds_seths_at(uint8_t hue, uint8_t sat, uint8_t pos) {
    rgblight_sethsv_at(hue, sat, _leds_val, pos);
}

void leds_off_at(uint8_t pos) {
    rgblight_sethsv_at(0, 0, 0, pos);
}

void leds_seths_range(uint8_t hue, uint8_t sat, uint8_t start, uint8_t end) {
    rgblight_sethsv_range(hue, sat, _leds_val, start, end);
}

void leds_off_range(uint8_t start, uint8_t end) {
    rgblight_sethsv_range(0, 0, 0, start, end);
}

uint8_t effect_step = 0;
static deferred_token effect_token = 0;

uint32_t leds_gyro_callback(uint32_t trigger_time, void *cb_arg) {
    rgblight_sethsv_at(HS_WHITE, 255, effect_step % 3);
    rgblight_sethsv_at(HS_BLUE, 255, (effect_step+1) % 3);
    rgblight_sethsv_at(HS_RED, 255, (effect_step+2) % 3);
    effect_step++;
    return 300;
}
void leds_gyro_start(void) {
    effect_step=0;
    if (effect_token != 0) cancel_deferred_exec(effect_token);
    effect_token = defer_exec(1, leds_gyro_callback, NULL);
}
void leds_gyro_stop(void) {
    if (effect_token != 0) cancel_deferred_exec(effect_token);
    effect_token = 0;
}



