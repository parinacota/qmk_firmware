#include QMK_KEYBOARD_H
#include "leds.h"
#include <ws2812.h>

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

void leds_on_for_at(uint16_t delay, uint8_t hue, uint8_t sat, uint8_t pos) {
    leds_on_for_range(delay, hue, sat, pos, pos+1);
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
uint16_t effect_delay = 500;
static deferred_token effect_token = 0;

uint32_t leds_jacks_effect(uint32_t trigger_time, void *cb_arg) {
    switch (effect_step) {
        case 0: //FR
            rgblight_sethsv_at(HS_BLUE, 255, LED_LEFT);
            rgblight_sethsv_at(HS_WHITE, 140, LED_BOTTOM);
            rgblight_sethsv_at(HS_RED, 255, LED_RIGHT);
            break;
        case 1: //DE
            rgblight_sethsv_at(HS_WHITE, 16, LED_LEFT);
            rgblight_sethsv_at(HS_YELLOW, 192, LED_BOTTOM);
            rgblight_sethsv_at(HS_RED, 255, LED_RIGHT);
            break;
        case 2: //IT
            rgblight_sethsv_at(HS_GREEN, 192, LED_LEFT);
            rgblight_sethsv_at(HS_WHITE, 140, LED_BOTTOM);
            rgblight_sethsv_at(HS_RED, 255, LED_RIGHT);
            break; 
        case 3: //ES
            rgblight_sethsv_at(HS_RED, 255, LED_LEFT);
            rgblight_sethsv_at(HS_YELLOW, 192, LED_BOTTOM);
            rgblight_sethsv_at(HS_RED, 255, LED_RIGHT);
            break;
        case 4: //LI
            rgblight_sethsv_at(HS_YELLOW, 192, LED_LEFT);
            rgblight_sethsv_at(HS_RED, 255, LED_BOTTOM);
            rgblight_sethsv_at(HS_GREEN, 192, LED_RIGHT);
            break; 
        case 5: //AT
            rgblight_sethsv_at(HS_RED, 255, LED_LEFT);
            rgblight_sethsv_at(HS_WHITE, 140, LED_BOTTOM);
            rgblight_sethsv_at(HS_RED, 255, LED_RIGHT);
            break;
    }
    effect_step = (effect_step + 1) % 6;
    if (/*effect_step == 0 &&*/ effect_delay > 10) {
        effect_delay = effect_delay * 0.95;
    }
    return effect_delay;
}

uint32_t leds_gyro_callback(uint32_t trigger_time, void *cb_arg) {
    rgblight_sethsv_at(HS_WHITE, 140, effect_step % 3);
    rgblight_sethsv_at(HS_BLUE, 255, (effect_step+1) % 3);
    rgblight_sethsv_at(HS_RED, 255, (effect_step+2) % 3);
    effect_step++;
    return effect_delay;
}

void leds_anim_start(uint8_t effect_id) {
    if (effect_token != 0) cancel_deferred_exec(effect_token);
    switch(effect_id) {
        case 1:
            effect_step = effect_step % 6;
            effect_delay = 800;
            effect_token = defer_exec(1, leds_jacks_effect, NULL); break;
        default:
            effect_step = effect_step % 3;
            effect_delay = 200;
            effect_token = defer_exec(1, leds_gyro_callback, NULL); break;
    }
    
}
void leds_anim_stop(void) {
    if (effect_token != 0) cancel_deferred_exec(effect_token);
    effect_token = 0;
}



