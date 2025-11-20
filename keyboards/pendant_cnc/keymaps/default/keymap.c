// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#define CNC_START_JOB       FR_TILD
#define CNC_PAUSE_JOB       FR_EXLM
#define CNC_STOP_JOB        FR_AT
#define CNC_DO_HOMING       C(A(G(FR_H)))
#define CNC_DO_PROBE        C(FR_P)
#define CNC_PRESET_PRECISE  S(FR_X)
#define CNC_PRESET_NORMAL   S(FR_C)
#define CNC_PRESET_RAPID    S(FR_V)
#define CNC_JOG_X_PLS       S(KC_RIGHT)
#define CNC_JOG_X_MNS       S(KC_LEFT)
#define CNC_JOG_Y_PLS       S(KC_UP)
#define CNC_JOG_Y_MNS       S(KC_DOWN)
#define CNC_JOG_Z_PLS       S(KC_PGUP)
#define CNC_JOG_Z_MNS       S(KC_PGDN)
#define CNC_JOG_STOP        S(FR_H)
#define CNC_GOTO_X0         S(FR_S)
#define CNC_GOTO_Y0         S(FR_D)
#define CNC_GOTO_Z0         S(FR_F)
#define CNC_GOTO_XY0        S(FR_A)
#define CNC_SET_X_0         S(FR_W)
#define CNC_SET_Y_0         S(FR_E)
#define CNC_SET_Z_0         S(FR_R)
#define CNC_UNLOCK          FR_DLR

#define PROFILE_SEND_TIMEOUT 30000 //resend profile selection after 30 seconds

#include QMK_KEYBOARD_H
#include "print.h"
#include "analog.h"
#include "keymap_french_mac_iso.h"

#define LED_SETZERO GP12
#define LED_LARGE_MOVE GP13
#define LED_MEDIUM_MOVE GP14
#define LED_SMALL_MOVE GP15
#define LED_X GP16
#define LED_Y GP17
#define LED_Z GP18

#define JOYSTICK_X GP27
#define JOYSTICK_Y GP26

enum layers {
    _DFT,
    _ZERO,
};

enum keycodes {
  KC_SETZERO_X = QK_USER,
  KC_SETZERO_Y,
  KC_SETZERO_Z,

  KC_AXIS_X,
  KC_AXIS_Y,
  KC_AXIS_Z,

  ENC_STEP_LEFT,
  ENC_STEP_RIGHT,

  ENC_100_MINUS,
  ENC_100_PLUS,

  KC_START_JOB,
  KC_PAUSE_JOB,
  KC_STOP_JOB,
  KC_DO_PROBE,
  KC_DO_HOMING,

  KC_LED_QKBOOT,
};

// ****************************** AXIS ****************
typedef enum AXIS_SELECTION {
  AXIS_X,
  AXIS_Y,
  AXIS_Z
} axis_selection_t;
axis_selection_t axis_selection;

void select_axis(axis_selection_t a) {
  axis_selection = a;
  gpio_write_pin(LED_X, (axis_selection == AXIS_X));
  gpio_write_pin(LED_Y, (axis_selection == AXIS_Y));
  gpio_write_pin(LED_Z, (axis_selection == AXIS_Z));
}

// ****************** STEPS SELECTION ****************
static uint32_t profile_last_use = 0; 
typedef enum STEPS_SELECTION {
  PROFILE_PRECISE,
  PROFILE_NORMAL,
  PROFILE_RAPID
} steps_selection_t;
steps_selection_t profile_selection;

void select_profile(steps_selection_t s) {
  profile_selection = s;
  profile_last_use = timer_read32();
  switch (profile_selection) {
    case PROFILE_PRECISE:  tap_code16(CNC_PRESET_PRECISE); break;
    case PROFILE_NORMAL: tap_code16(CNC_PRESET_NORMAL); break;
    case PROFILE_RAPID:  tap_code16(CNC_PRESET_RAPID); break;
  }
  gpio_write_pin(LED_SMALL_MOVE, (profile_selection == PROFILE_PRECISE));
  gpio_write_pin(LED_MEDIUM_MOVE, (profile_selection == PROFILE_NORMAL));
  gpio_write_pin(LED_LARGE_MOVE, (profile_selection == PROFILE_RAPID));
}

void profile_inc(void) {
  switch (profile_selection) {
    case PROFILE_PRECISE:
      select_profile(PROFILE_NORMAL);
      break;
    case PROFILE_NORMAL:
      select_profile(PROFILE_RAPID);
      break;
    case PROFILE_RAPID:
      break;
  }
}

void profile_dec(void) {
  switch (profile_selection) {
    case PROFILE_PRECISE:
      break;
    case PROFILE_NORMAL:
      select_profile(PROFILE_PRECISE);
      break;
    case PROFILE_RAPID:
      select_profile(PROFILE_NORMAL);
      break;
  }
}

void profile_check(void) {
  if (timer_elapsed32(profile_last_use) > PROFILE_SEND_TIMEOUT) {
    select_profile(profile_selection);
  }
}

uint32_t profile_callback(uint32_t trigger_time, void *cb_arg) {
    /* do something */
    select_profile(profile_selection);
    return 0;
}

// ********* JOYSTICK *******
joystick_config_t joystick_axes[JOYSTICK_AXIS_COUNT] = {
  JOYSTICK_AXIS_IN(JOYSTICK_X, 788, 430, 100),
  JOYSTICK_AXIS_IN(JOYSTICK_Y, 180, 530, 886)
};

// ******** KEYBOARD *********
void keyboard_pre_init_user(void) {
  // Pin directions
  setPinOutput(LED_SETZERO);
  setPinOutput(LED_LARGE_MOVE);
  setPinOutput(LED_MEDIUM_MOVE);
  setPinOutput(LED_SMALL_MOVE);
  setPinOutput(LED_X);
  setPinOutput(LED_Y);
  setPinOutput(LED_Z);
}

void keyboard_post_init_user(void) {
  // default values
  select_axis(AXIS_X);
  select_profile(PROFILE_NORMAL);
  defer_exec(300, profile_callback, NULL);
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_DFT] = LAYOUT_cnc(
      LT(0,KC_START_JOB),     LT(0,KC_PAUSE_JOB),     LT(0,KC_STOP_JOB),
      LT(0,KC_DO_HOMING),     KC_NO,                  LT(0,KC_DO_PROBE), 
      LT(_ZERO, 0),                                   KC_NO,    
      LT(0,KC_AXIS_X),        LT(0,KC_AXIS_Y),        LT(0,KC_AXIS_Z)    
    ),

    [_ZERO] = LAYOUT_cnc(
      XXXXXXX,                XXXXXXX,                CNC_UNLOCK,   
      XXXXXXX,                XXXXXXX,                XXXXXXX, 
      _______,                                        KC_LED_QKBOOT,    
      KC_SETZERO_X,           KC_SETZERO_Y,           KC_SETZERO_Z    
    )
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_DFT] = {  ENCODER_CCW_CW(ENC_100_MINUS, ENC_100_PLUS), 
                ENCODER_CCW_CW(ENC_STEP_RIGHT, ENC_STEP_LEFT) },
    [_ZERO] = { ENCODER_CCW_CW(XXXXXXX, XXXXXXX), 
                ENCODER_CCW_CW(XXXXXXX, XXXXXXX) },
};
#endif

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

  if (record->event.pressed) {
    switch (keycode) {

      // MAIN ENCODER
      case ENC_100_PLUS:
        profile_check();
        switch (axis_selection) {
          case AXIS_X: tap_code16(CNC_JOG_X_PLS); break;
          case AXIS_Y: tap_code16(CNC_JOG_Y_PLS); break;
          case AXIS_Z: tap_code16(CNC_JOG_Z_PLS); break;
        }
        return true;
      case ENC_100_MINUS:
        profile_check();
        switch (axis_selection) {
          case AXIS_X: tap_code16(CNC_JOG_X_MNS); break;
          case AXIS_Y: tap_code16(CNC_JOG_Y_MNS); break;
          case AXIS_Z: tap_code16(CNC_JOG_Z_MNS); break;
        }
        return false;

      // PROFILE ENCODER
      case ENC_STEP_LEFT:
        profile_dec();
        return false; // Skip all further processing of this keycode
      case ENC_STEP_RIGHT:
        profile_inc();
        return false; // Skip all further processing of this keycode

      // AXES X,Y and Z
      case LT(0,KC_AXIS_X):
        if (record->tap.count) select_axis(AXIS_X); // Intercept tap function to select X
        else tap_code16(CNC_GOTO_X0);               // Intercept hold function to go to X0
        return false;
      case LT(0,KC_AXIS_Y):
        if (record->tap.count) select_axis(AXIS_Y); // Intercept tap function to select Y
        else tap_code16(CNC_GOTO_Y0);               // Intercept hold function to go to Y0
        return false;
      case LT(0,KC_AXIS_Z):
        if (record->tap.count) select_axis(AXIS_Z); // Intercept tap function to select Z
        else tap_code16(CNC_GOTO_Z0);               // Intercept hold function to go to Z0
        return false;

      // SET ZERO X, Y and Z
      case LT(_ZERO, 0):
        if (record->tap.count) { tap_code16(CNC_JOG_STOP); return false;}
        return true;
      case KC_SETZERO_X:
        tap_code16(CNC_SET_X_0);
        gpio_write_pin(LED_X, false);
        return false;
      case KC_SETZERO_Y:
        tap_code16(CNC_SET_Y_0);
        gpio_write_pin(LED_Y, false);
        return false;
      case KC_SETZERO_Z:
        tap_code16(CNC_SET_Z_0);
        gpio_write_pin(LED_Z, false);
        return false;

      // START, PAUSE and STOP
      case LT(0,KC_START_JOB):
        if (record->tap.count) {} // Intercept tap function 
        else tap_code16(CNC_START_JOB);               // Intercept hold function 
        return false;
      case LT(0,KC_PAUSE_JOB):
        if (record->tap.count) {} // Intercept tap function 
        else tap_code16(CNC_PAUSE_JOB);               // Intercept hold function 
        return false;
      case LT(0,KC_STOP_JOB):
        if (record->tap.count) tap_code16(CNC_JOG_STOP); // Intercept tap function 
        else tap_code16(CNC_STOP_JOB);               // Intercept hold function 
        return false;

      // HOMING and PROBE
      case LT(0,KC_DO_HOMING):
        if (record->tap.count) tap_code16(CNC_GOTO_XY0); // Intercept tap function 
        else tap_code16(CNC_DO_HOMING);                 // Intercept hold function 
        return false;
      case LT(0,KC_DO_PROBE):
        if (record->tap.count) {} // Intercept tap function 
        else tap_code16(CNC_DO_PROBE);               // Intercept hold function 
        return false;

        case KC_LED_QKBOOT:
          gpio_write_pin(LED_SMALL_MOVE, true);
          gpio_write_pin(LED_MEDIUM_MOVE, true);
          gpio_write_pin(LED_LARGE_MOVE, true);
          bootloader_jump();
          return true;
        
    }
  }
  return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
      case _ZERO:
        gpio_write_pin(LED_SETZERO, true);
        gpio_write_pin(LED_SMALL_MOVE, false);
        gpio_write_pin(LED_MEDIUM_MOVE, false);
        gpio_write_pin(LED_LARGE_MOVE, false);
        gpio_write_pin(LED_X, true);
        gpio_write_pin(LED_Y, true);
        gpio_write_pin(LED_Z, true);
        break;
      case _DFT:
        gpio_write_pin(LED_SMALL_MOVE, (profile_selection == PROFILE_PRECISE));
        gpio_write_pin(LED_MEDIUM_MOVE, (profile_selection == PROFILE_NORMAL));
        gpio_write_pin(LED_LARGE_MOVE, (profile_selection == PROFILE_RAPID));
        gpio_write_pin(LED_X, (axis_selection == AXIS_X));
        gpio_write_pin(LED_Y, (axis_selection == AXIS_Y));
        gpio_write_pin(LED_Z, (axis_selection == AXIS_Z));
        gpio_write_pin(LED_SETZERO, false);
        break;
    }
    return state;
}

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        // LONG press for GOTO X0, Y0, Z0 and STOP, HOMING and PROBE
        case LT(0,KC_AXIS_X):
        case LT(0,KC_AXIS_Y):
        case LT(0,KC_AXIS_Z):
        case LT(0,KC_STOP_JOB):
        case LT(0,KC_DO_HOMING):
        case LT(0,KC_DO_PROBE):
            return 700;
        // otherwise short press
        default:
            return TAPPING_TERM;
    }
}