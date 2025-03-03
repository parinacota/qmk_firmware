// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include <ws2812.h>
#include <keymap_french.h>
#include <emul_azerty.h>
// #include "drv2605l.h"

static uint16_t trackpoint_timer;
extern int tp_buttons; // mousekey button state set in action.c and used in ps2_mouse.c

typedef enum tp_lock_dirs {
  TP_FREE,
  TP_LOCK_H,
  TP_LOCK_V,
} tp_lock_dirs_t;
static tp_lock_dirs_t trackpoint_lock_dir;

#define LED_INTERNAL  GP17
#define SOLENOID GP6
#define SOLENOID_RING_DEFAULT_TIME 60
bool solenoid_enabled = true;
int solenoid_ring_time = SOLENOID_RING_DEFAULT_TIME;

enum layers {
    _BAS,
    _SHF,
    _1DK,
    _SDK,
    _COD,
    _NUM,
    _FCT,
    _MSE
};

enum custom_keycodes {
  MA_TOBASE = ME_LAST_EMUL,
  MA_CMD_ALT_TAB,
  //FUNCTION LAYER
  ME_MICOFF,
  ME_PSCR,
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
    TD2_BSPC_MS3,
    TD3_SLSH_LALT,
};

// Tap Dance definitions
tap_dance_action_t tap_dance_actions[] = {
    // Tap once for Escape, twice for Caps Lock
    [TD1_CTL_GUI] = EMUL_ACTION_TAP_DANCE_DOUBLE_INVERT_OSX(KC_LCTL, KC_LGUI),
    [TD2_BSPC_MS3] = ACTION_TAP_DANCE_TAP_HOLD(KC_BSPC, MS_BTN3),
    [TD3_SLSH_LALT] = ACTION_TAP_DANCE_TAP_HOLD(S(KC_DOT), KC_LALT),
};

void keyboard_pre_init_user(void) {
  //internal led
  gpio_set_pin_output(LED_INTERNAL);
  gpio_write_pin(LED_INTERNAL,true);

  //led RGB
  ws2812_init();

  //Solenoid
  gpio_set_pin_output(SOLENOID);
  gpio_write_pin(SOLENOID,false);

  //RESET PS2 - GP9 = PIN RESET PS2
  gpio_set_pin_output(GP9);
  gpio_write_pin(GP9,true);
  wait_ms(2);
  gpio_write_pin(GP9,false);
}

bool solenoid_on = false;

uint32_t solenoid_off_callback(uint32_t trigger_time, void *cb_arg) {
  gpio_write_pin(SOLENOID, false);
  return 0;
}

void solenoid_ring(void) {
  gpio_write_pin(SOLENOID, true);
  defer_exec(solenoid_ring_time, solenoid_off_callback, false);
}

void solenoid_enable(bool enabled) {
  solenoid_enabled = enabled;
  if (solenoid_enabled) solenoid_ring();
}

void solenoid_ring_time_inc(void) {
  solenoid_enabled = true;
  if (solenoid_ring_time < 150) solenoid_ring_time += 5;
  solenoid_ring();
}

void solenoid_ring_time_dec(void) {
  solenoid_enabled = true;
  if (solenoid_ring_time > 10) solenoid_ring_time -= 5;
  solenoid_ring();
}

void layer_mouse_feedback(bool enabled) {
  if (enabled) {
    ws2812_set_color_all(255, 0, 0);
  } else {
    if (solenoid_enabled) solenoid_ring();
    if (layer_state_is(_1DK) || layer_state_is(_SDK))
      ws2812_set_color_all(255, 127, 127);
    else
    ws2812_set_color_all(0, 0, 0);
  }
}

uint32_t turn_off_led(uint32_t trigger_time, void *cb_arg) {
    ws2812_set_color_all(0, 0, 0);
    return 0;
}

void keyboard_post_init_user(void) {
  // turn off internal led + detect host os
  gpio_write_pin(LED_INTERNAL,false);
  //rgblight_sethsv_at(HSV_RED, 0);
  ws2812_set_color_all(255, 0, 0);
  emul_keyboard_post_init_user();
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_BAS] = LAYOUT_36keys(
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+
FR_Q,         FR_C,         FR_O,         FR_P,         FR_W,                                     FR_J,         FR_M,         FR_D,         OSL(_1DK),    FR_Y,   
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+ 
FR_A,         FR_S,         FR_E,         FR_N,         FR_F,                                     FR_L,         FR_R,         FR_T,         FR_I,         FR_U,
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+ 
LALT_T(FR_Z), FR_X ,        ME_MINS,      FR_V,         LT(_FCT,FR_B),                            FR_DOT,       FR_H,         FR_G,         FR_COMM,      LALT_T(FR_K),
//-----------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
                                        TD(TD1_CTL_GUI),LM(_SHF, MOD_LSFT),MO(_COD),TD(TD2_BSPC_MS3),LT(_NUM,KC_SPC),KC_ENT        
//                                       +-------------+-------------+-------------+-------------+-------------+-------------+     
    ),

    [_SHF] = LAYOUT_36keys(
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+
_______,      _______,      _______,      _______,      _______,                                  _______,      _______,      _______,      ME_EXCL,      _______,   
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+ 
_______,      _______,      _______,      _______,      _______,                                  _______,      _______,      _______,      _______,      _______,
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+ 
_______,      _______,      FR_COMM,      _______,      FR_B,                                     ME_COLN,      _______,      _______,      ME_SCLN,      FR_K,
//-----------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
                                          _______,      _______,      _______,      ME_DEL,       ME_INSEC,     _______        
//                                       +-------------+-------------+-------------+-------------+-------------+-------------+     
    ),
 

    [_1DK] = LAYOUT_36keys(
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+
ME_ACIR,      FR_CCED,      ME_OE,        ME_OCIR,      FR_DEG,                                   ME_SECT,      ME_MICR,      ME_UNDS,      FR_DIAE,      ME_UCIR,   
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+ 
FR_AGRV,      FR_EACU,      FR_EGRV,      ME_ECIR,      ME_NTILD,                                 ME_LQUOTFR,   ME_RQUOTFR,   ME_ICIR,      ME_ITRE,      FR_UGRV,
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+ 
ME_AE,        ME_SUP2,      ME_QCADR,     ME_SCADR,     LT(_FCT,ME_CADR),                         ME_ETC,       ME_https,     XXXXXXX,      XXXXXXX,      ME_COPYR,
//-----------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
                                          _______,      OSL(_SDK),    XXXXXXX,      XXXXXXX,      ME_QUOTFR,    XXXXXXX        
//                                       +-------------+-------------+-------------+-------------+-------------+-------------+     
    ),

    [_SDK] = LAYOUT_36keys(
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+
ME_ACIRM,     ME_CCEDM,     ME_OEM,       ME_OCIRM,     ME_BULLET,                                XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      ME_UCIRM,   
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+ 
ME_AGRVM,     ME_EACUM,     ME_EGRVM,     ME_ECIRM,     ME_NTILDM,                                XXXXXXX,      XXXXXXX,      ME_ICIRM,     ME_ITREM,     ME_UGRVM,
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+ 
ME_AEM,       ME_SUP3,      XXXXXXX,      XXXXXXX,      XXXXXXX,                                  XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      ME_RGSTRD,
//-----------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
                                          _______,      XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX        
//                                       +-------------+-------------+-------------+-------------+-------------+-------------+     
    ),

    [_COD] = LAYOUT_36keys(
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+
FR_DQUO,      ME_LABK,      ME_RABK,      FR_AMPR,      FR_PERC,                                  FR_DLR,       ME_EURO,      ME_SML1,      ME_SML2,      ME_SML3,   
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+ 
ME_LCBR,      FR_LPRN,      FR_RPRN,      ME_RCBR,      ME_PIPE,                                  KC_LEFT,      KC_DOWN,      KC_UP,        KC_RGHT,      XXXXXXX,
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+ 
FR_QUOT,      ME_LBRC,      ME_RBRC,      ME_UNDS,      ME_HASH,                                  KC_HOME,      KC_PGDN,      KC_PGUP,      KC_END,       XXXXXXX,
//-----------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
                                          _______,      XXXXXXX,      XXXXXXX,      _______,      KC_LSFT,      _______        
//                                       +-------------+-------------+-------------+-------------+-------------+-------------+     
    ),

    [_NUM] = LAYOUT_36keys(
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+
KC_TAB,       ME_AT,        KC_SPC,       ME_TIL,       KC_ESC,                                   XXXXXXX,      FR_DOT,       FR_COMM,      XXXXXXX,      ME_BSLH,   
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+ 
FR_1,         FR_2,         FR_3,         FR_4,         FR_5,                                     FR_6,         FR_7,         FR_8,         FR_9,         FR_0,
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+ 
MA_CMD_ALT_TAB,FR_LPRN,     FR_RPRN,      ME_CIR,       ME_GRV,                                   ME_EQL,       ME_PLUS,      ME_MINS,      ME_ASTR,      FR_SLSH,
//-----------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
                                          _______,      KC_LSFT,      _______,      _______,      _______,      _______        
//                                       +-------------+-------------+-------------+-------------+-------------+-------------+     
    ),

    [_FCT] = LAYOUT_36keys(
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+
QK_REBOOT,    SOL_INC,      ME_REPEAT_TOGGLE,KC_BRIU,   KC_VOLU,                                  KC_F1,        KC_F2,        KC_F3,        KC_F4,        KC_F5,   
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+ 
QK_BOOTLOADER,SOL_DEC,      XXXXXXX,      KC_BRID,      KC_VOLD,                                  KC_F6,        KC_F7,        KC_F8,        KC_F9,        KC_F10,  
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+ 
XXXXXXX,      SOL_TGGL,     XXXXXXX,      XXXXXXX,      XXXXXXX,                                  KC_F11,       KC_F12,       XXXXXXX,      XXXXXXX,      ME_PSCR,  
//-----------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
                                          XXXXXXX,      XXXXXXX,      XXXXXXX,      KC_MUTE,      ME_MICOFF,    XXXXXXX        
//                                       +-------------+-------------+-------------+-------------+-------------+-------------+     
    ),

    [_MSE] = LAYOUT_36keys(
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+
_______,      _______,      _______,      _______,      _______,                                  _______,      _______,      _______,      _______,      _______,   
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+ 
_______,      _______,      _______,      _______,      _______,                                  _______,      _______,      _______,      _______,      _______,
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+ 
_______,      _______,      _______,      _______,      _______,                                  _______,      _______,      _______,      _______,      _______,
//-----------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
                                          _______,      _______,      _______,      _______,      MS_BTN1,      MS_BTN2        
//                                       +-------------+-------------+-------------+-------------+-------------+-------------+     
    )
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  tap_dance_action_t *action;

  //release MOUSE layer if any other key is pressed
  if (trackpoint_timer && keycode!=MS_BTN1 && keycode!=MS_BTN2 && keycode!=MS_BTN3) { 
      layer_off(_MSE);
      layer_mouse_feedback(false);
      unregister_code(MS_BTN1);
      unregister_code(MS_BTN2);
      unregister_code(MS_BTN3);
      trackpoint_timer = 0; //Reset the timer again until the mouse moves more
  }

  // MOUSE btns
  if (keycode == MS_BTN1 || keycode == MS_BTN2) {
    if (record->event.pressed && solenoid_enabled) solenoid_ring();
    trackpoint_timer = timer_read(); 
    //enable _NUM when MS_BTN1 is hold
    if (keycode == MS_BTN1) {
      if (record->event.pressed) layer_on(_NUM);
      else layer_off(_NUM);
    }

  }

  //AZERTY EMUL
  process_record_user_emul(keycode, record);

  //KEYS
  switch (keycode) {
    //TAP DANCE section
    case TD(TD2_BSPC_MS3):
    case TD(TD3_SLSH_LALT):
      action = &tap_dance_actions[QK_TAP_DANCE_GET_INDEX(keycode)];
      if (!record->event.pressed && action->state.count && !action->state.finished) {
        tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
        tap_code16(tap_hold->tap);
      }
      break;

    // OTHER KEYS
    default:
      //OTHER THAN AZERTY EMUL
      if (record->event.pressed) {
        switch (keycode) {
          case MA_CMD_ALT_TAB: if (emul_get_os() == EMUL_OS_OSX) SEND_STRING(SS_LGUI(SS_TAP(X_TAB)));      else SEND_STRING(SS_LALT(SS_TAP(X_TAB)));   break;
        // FUNCT
          case ME_PSCR:   if (emul_get_os() == EMUL_OS_OSX)   SEND_STRING(SS_LSFT(SS_LCMD(SS_TAP(X_4))));  else SEND_STRING(SS_TAP(X_PSCR));           break;
          case ME_MICOFF: if (emul_get_os() == EMUL_OS_OSX)   SEND_STRING(SS_LCMD(SS_TAP(X_F4)));          else SEND_STRING(SS_LWIN(SS_LALT(SS_TAP(X_K))));    break;
          case SOL_TGGL:  solenoid_enable(!solenoid_enabled); break;
          case SOL_INC:   solenoid_ring_time_inc(); break;
          case SOL_DEC:   solenoid_ring_time_dec(); break;
      }
    }
  }
  return true;
}

// LED for some layers
layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
      case _1DK:
      case _SDK:   
        ws2812_set_color_all(255, 127, 127);
        break;
      case _FCT: 
        ws2812_set_color_all(255, 255, 0);
        break;
      case _MSE: 
        layer_mouse_feedback(true);
        break;
      default: 
        ws2812_set_color_all(0, 0, 0);
    }
    return state;
}

void ps2_mouse_moved_user(report_mouse_t *mouse_report) { // Whenever the TrackPoint starts moving, check if the timer exists.
    if (trackpoint_timer) {
        trackpoint_timer = timer_read();
    } else {
        if (!tp_buttons) { //I'm still a bit confused about this one, but I believe it checks that if the mousekey state isn't set, turn on this layer specified?
            layer_on(_MSE);
            trackpoint_timer = timer_read();
        }
    }
    //lock H or V if _COD is enabled
    if (layer_state_is(_COD)) {
      if (trackpoint_lock_dir == TP_FREE) {
        if (abs(mouse_report->x) > abs(mouse_report->y))
          trackpoint_lock_dir = TP_LOCK_H;
        else
          trackpoint_lock_dir = TP_LOCK_V;
      }
    } else {    
      trackpoint_lock_dir = TP_FREE;
    }

    //if locked
    if (trackpoint_lock_dir == TP_LOCK_H) mouse_report->y = 0;
    if (trackpoint_lock_dir == TP_LOCK_V) mouse_report->x = 0;
}

void matrix_scan_user(void) {  // ALWAYS RUNNING VOID FUNCTION, CAN BE USED TO CHECK CLOCK RUNTIMES OVER THE DURATION THAT THE KEYBOARD IS POWERED ON
  if (trackpoint_timer && (timer_elapsed(trackpoint_timer) > MOUSE_LAYER_TIMEOUT)) { //If the time of both the TP timer
    if (!tp_buttons) {
      layer_off(_MSE);
      layer_mouse_feedback(false);
      trackpoint_timer = 0; //Reset the timer again until the mouse moves more
      trackpoint_lock_dir = TP_FREE;
    }
  }
  matrix_scan_user_emul();
}

