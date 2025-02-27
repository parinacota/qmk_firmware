// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "drv2605l.h"

bool macos_mode = false;

//#define LED_INTERNAL  GP17
#define PS2RESETPIN GP14

enum layers {
    _BAS,
    _NUM,
    _FCT,
};

enum custom_keycodes {
  MA_TOBASE = SAFE_RANGE,
    //FUNCTION LAYER
    ME_MICOFF,
    SOL_TGGL,
    SOL_INC,
    SOL_DEC,
};

typedef struct {
    uint16_t tap;
    uint16_t hold;
    uint16_t held;
} tap_dance_tap_hold_t;

void tap_dance_tap_hold_finished(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (state->pressed) {
        if (state->count == 1
#ifndef PERMISSIVE_HOLD
            && !state->interrupted
#endif
        ) {
            register_code16(tap_hold->hold);
            tap_hold->held = tap_hold->hold;
        } else {
            register_code16(tap_hold->tap);
            tap_hold->held = tap_hold->tap;
        }
    }
}

void tap_dance_tap_hold_reset(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (tap_hold->held) {
        unregister_code16(tap_hold->held);
        tap_hold->held = 0;
    }
}

#define ACTION_TAP_DANCE_TAP_HOLD(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished, tap_dance_tap_hold_reset}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }

// Tap dance enums
enum {
    TD1_CTL_GUI,
};

// Tap Dance definitions
tap_dance_action_t tap_dance_actions[] = {
    // Tap once for Escape, twice for Caps Lock
    [TD1_CTL_GUI] = ACTION_TAP_DANCE_DOUBLE(KC_LCTL, KC_LGUI),
};

void keyboard_pre_init_user(void) {
  //nothing
}

uint32_t turn_off_led(uint32_t trigger_time, void *cb_arg) {
    rgblight_sethsv_range(HSV_OFF, 0, 3);
    return 0;
}

uint32_t get_host_os(uint32_t trigger_time, void *cb_arg) {
  switch(detected_host_os()) {
    
    case OS_MACOS:
    case OS_IOS:
      macos_mode = true;
      rgblight_sethsv_range(HSV_BLUE, 0, 3);
      defer_exec(1500, turn_off_led, false);
      break;
    default:
      macos_mode = false;
      rgblight_sethsv_range(HSV_WHITE, 0, 3);
      defer_exec(1500, turn_off_led, false);
      break;
  }
  return 0;
}

void keyboard_post_init_user(void) {
  // detect host os
  
  defer_exec(500, get_host_os, NULL);
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_BAS] = LAYOUT_36keys(
      KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                               KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,
      KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                               KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN,
LALT_T(KC_Z),  KC_X,    KC_C,    KC_V,    LT(_FCT,KC_B),                      KC_N,    KC_M,    KC_COMM, KC_DOT,  LALT_T(KC_SLSH),
                        TD(TD1_CTL_GUI),KC_LSFT, KC_ALGR,            KC_BSPC, LT(_NUM,KC_SPC), KC_ENT  
  
    ),

    [_NUM] = LAYOUT_36keys(
      KC_TAB,    _______,    _______,      _______,  KC_ESC,                               KC_PPLS, KC_7,   KC_8,   KC_9,   KC_EQL,
      KC_LEFT,   KC_UP,      KC_DOWN,      KC_RIGHT, _______,                              KC_0,    KC_4,   KC_5,   KC_6,   KC_MINS,
      LALT_T(KC_HOME),   KC_PAGE_UP, KC_PAGE_DOWN, KC_END,   _______,                              KC_MINS, KC_1,   KC_2,   KC_3,   LALT_T(KC_DOT),
                                          _______,  _______, _______,             _______, _______, _______                         
    ),

    [_FCT] = LAYOUT_36keys(
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+
QK_REBOOT,    SOL_INC,      XXXXXXX,      KC_BRIU,      KC_VOLU,                                  KC_F1,        KC_F2,        KC_F3,        KC_F4,        KC_F5,   
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+ 
QK_BOOTLOADER,SOL_DEC,      XXXXXXX,      KC_BRID,      KC_VOLD,                                  KC_F6,        KC_F7,        KC_F8,        KC_F9,        KC_F10,  
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+ 
XXXXXXX,      SOL_TGGL,     XXXXXXX,      XXXXXXX,      XXXXXXX,                                  KC_F11,       KC_F12,       XXXXXXX,      XXXXXXX,      KC_PSCR,  
//-----------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
                                          XXXXXXX,      XXXXXXX,      XXXXXXX,      KC_MUTE,      XXXXXXX,      XXXXXXX        
//                                       +-------------+-------------+-------------+-------------+-------------+-------------+     
    )
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  //tap_dance_action_t *action;

  switch (keycode) {
    //TAP DANCE section
    /* ENABLE for TAP_DANCE with tap */
    /*
    case TD(TD3_SLSH_LALT):
      action = &tap_dance_actions[QK_TAP_DANCE_GET_INDEX(keycode)];
      if (!record->event.pressed && action->state.count && !action->state.finished) {
        tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
        tap_code16(tap_hold->tap);
      }
      break;
    */

    // OTHER KEYS
    default:
      if (record->event.pressed) {
        switch (keycode) {
          case SOL_TGGL:  /*solenoid_enable(!solenoid_enabled);*/ break;
          case SOL_INC:   /*solenoid_ring_time_inc(); */break;
          case SOL_DEC:   /*solenoid_ring_time_dec(); */break;
        }
      }  
  }
  return true;
}

// LED for some layers
layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
      case _NUM:
      rgblight_sethsv_range(HSV_PURPLE, 0, 3); break;
      case _FCT: 
      rgblight_sethsv_range(HSV_ORANGE, 0, 3);
        break;
      default: 
      rgblight_sethsv_range(HSV_OFF, 0, 3);
    }
    return state;
}

bool get_haptic_enabled_key(uint16_t keycode, keyrecord_t *record) {
  return false;
}

