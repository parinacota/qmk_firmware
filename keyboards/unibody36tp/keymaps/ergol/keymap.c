// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include <lib/leds.h>
#include <lib/haptic.h>
#include <lib/trackpoint.h>

#include <lib/emul_azerty.h>

static uint16_t trackpoint_timer;
extern int tp_buttons; // mousekey button state set in action.c and used in ps2_mouse.c
typedef enum tp_lock_dirs {
  TP_FREE,
  TP_LOCK_H,
  TP_LOCK_V,
} tp_lock_dirs_t;
static tp_lock_dirs_t trackpoint_lock_dir;

enum layers { _BAS, _SHF, _1DK, _SDK, _COD, _NUM, _FCT, _MSE, _OTH };

enum custom_keycodes {
  MA_TOBASE = NEW_SAFE_RANGE,
  MA_CMD_ALT_TAB,
  //FUNCTION LAYER
  KB_MICOFF,
  KB_PSCR,
  LED_UP,
  LED_DOWN,
  HAPT_INC,
  HAPT_DEC,
  HAPT_TGGL,
  LED_FR,
  LED_EU,
  AZ_LINUX
};

// ########### TAP DANCE #############
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

void dance_flsh_each(tap_dance_state_t *state, void *user_data) {
  switch (state->count) {
      case 1:
          leds_seths_at(HS_RED, LED_LEFT); break;
          break;
      case 2:
          leds_seths_at(HS_RED, LED_RIGHT); break;
          break;
      case 3:
          leds_seths_at(HS_RED, LED_BOTTOM); break;
          break;
      case 4:
          leds_off_at(LED_LEFT);
          wait_ms(50);
          leds_off_at(LED_RIGHT);
          wait_ms(50);
          leds_off_at(LED_BOTTOM);
  }
}

// On the fourth tap, set the keyboard on flash state
void dance_flsh_finished(tap_dance_state_t *state, void *user_data) {
  if (state->count >= 4) {
      //reset_keyboard();
  }
}

// If the flash state didn't happen, then turn off LEDs, left to right
void dance_flsh_reset(tap_dance_state_t *state, void *user_data) {
  leds_off_at(LED_LEFT);
  wait_ms(50);
  leds_off_at(LED_RIGHT);
  wait_ms(50);
  leds_off_at(LED_BOTTOM);
}

#define ACTION_TAP_DANCE_TAP_HOLD(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished, tap_dance_tap_hold_reset}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }

// Tap dance enums
enum { TD1_CTL_GUI, TD2_BSPC_MS3, TD_SAFE_FLASH};

// Tap Dance definitions
tap_dance_action_t tap_dance_actions[] = {
  // Tap once for Escape, twice for Caps Lock
  [TD1_CTL_GUI] = EMUL_ACTION_TAP_DANCE_DOUBLE_INVERT_OSX(KC_LCTL, KC_LGUI),
  [TD2_BSPC_MS3] = ACTION_TAP_DANCE_TAP_HOLD(KC_BSPC, MS_BTN3),
  [TD_SAFE_FLASH] = ACTION_TAP_DANCE_FN_ADVANCED(dance_flsh_each, dance_flsh_finished, dance_flsh_reset)
};

// ################ Utily #################
void emul_notify_event_callback(emul_event_t ev) {
  switch(ev) {
    case EMUL_EVENT_OS_OSX:   leds_on_for_range(500, HS_WHITE, 0, LED_COUNT); break;
    case EMUL_EVENT_OS_LINUX: leds_on_for_range(500, HS_RED, 0, LED_COUNT); break;
    case EMUL_EVENT_OS_WIN:   leds_on_for_at(500,HS_RED,LED_LEFT);leds_on_for_at(500,HS_GREEN,LED_RIGHT);leds_on_for_at(500,HS_BLUE,LED_BOTTOM); break;
    case EMUL_EVENT_REPEAT_ON: leds_on_for_range(500, HS_GREEN, LED_LEFT, LED_LEFT+1); break;
    case EMUL_EVENT_REPEAT_OFF: leds_on_for_range(500, HS_RED, LED_LEFT, LED_LEFT+1); break;
    default:            leds_on_for_range(500, HS_YELLOW, 0, LED_COUNT); break;
  }
}

// ######## QMK routines ########
void keyboard_pre_init_user(void) {
  //RESET PS2
  trackpoint_reset_ps2();
}

void keyboard_post_init_user(void) {
  leds_turn_off();
  emul_keyboard_post_init_user();
}

bool process_detected_host_os_kb(os_variant_t detected_os) {
  //leds_on_for_at(500,HS_WHITE,LED_RIGHT);
  return true;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [_BAS] = LAYOUT_36keys(
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+
AZ_Q,         AZ_C,         AZ_O,         AZ_P,         AZ_W,                                     AZ_J,         AZ_M,         AZ_D,         OSL(_1DK),    LT(_OTH,AZ_Y),   
//-----------+-------------+-------------+=============+-------------+                           +-------------+=============+-------------+-------------+-------------+ 
AZ_A,         AZ_S,         AZ_E,         AZ_N,         AZ_F,                                     AZ_L,         AZ_R,         AZ_T,         AZ_I,         AZ_U,
//-----------+-------------+-------------+=============+-------------+                           +-------------+=============+-------------+-------------+-------------+ 
LALT_T(AZ_Z), AZ_X ,        AZ_MINS,      AZ_V,         LT(_FCT,AZ_B),                            AZ_DOT,       AZ_H,         AZ_G,         AZ_COMM,      LALT_T(AZ_K),
//-----------+-------------+-------------+-------------+=============+-------------+-------------+=============+-------------+-------------+-------------+-------------+
                                        TD(TD1_CTL_GUI),LM(_SHF, MOD_LSFT),MO(_COD),TD(TD2_BSPC_MS3),LT(_NUM,KC_SPC),KC_ENT        
//                                       +-------------+=============+-------------+-------------+=============+-------------+     
    ),

    [_SHF] = LAYOUT_36keys(
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+
_______,      _______,      _______,      _______,      _______,                                  _______,      _______,      _______,      AZ_EXCL,      _______,   
//-----------+-------------+-------------+=============+-------------+                           +-------------+=============+-------------+-------------+-------------+ 
_______,      _______,      _______,      _______,      _______,                                  _______,      _______,      _______,      _______,      _______,
//-----------+-------------+-------------+=============+-------------+                           +-------------+=============+-------------+-------------+-------------+ 
_______,      _______,      AZ_COMM,      _______,      AZ_B,                                     AZ_COLN,      _______,      _______,      AZ_SCLN,      AZ_K,
//-----------+-------------+-------------+-------------+=============+-------------+-------------+=============+-------------+-------------+-------------+-------------+
                                          _______,      _______,      _______,      AZ_DEL,       AZ_INSEC,     _______        
//                                       +-------------+=============+-------------+-------------+=============+-------------+     
    ),


    [_1DK] = LAYOUT_36keys(
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+
AZ_ACIR,      AZ_CCED,      AZ_OE,        AZ_OCIR,      AZ_DEG,                                   AZ_SECT,      AZ_MICR,      AZ_UNDS,      AZ_DIAE,      AZ_UCIR,   
//-----------+-------------+-------------+=============+-------------+                           +-------------+=============+-------------+-------------+-------------+ 
AZ_AGRV,      AZ_EACU,      AZ_EGRV,      AZ_ECIR,      AZ_NTILD,                                 AZ_LQUOTFR,   AZ_RQUOTFR,   AZ_ICIR,      AZ_ITRE,      AZ_UGRV,
//-----------+-------------+-------------+=============+-------------+                           +-------------+=============+-------------+-------------+-------------+ 
AZ_AE,        AZ_SUP2,      AZ_QCADR,     AZ_SCADR,     AZ_CADR,                                  AZ_ETC,       AZ_https,     XXXXXXX,      XXXXXXX,      AZ_COPYR,
//-----------+-------------+-------------+-------------+=============+-------------+-------------+=============+-------------+-------------+-------------+-------------+
                                          _______,      OSL(_SDK),    TO(_BAS),      TO(_BAS),    AZ_QUOTFR,    XXXXXXX        
//                                       +-------------+=============+-------------+-------------+=============+-------------+     
    ),

    [_SDK] = LAYOUT_36keys(
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+
AZ_ACIRM,     AZ_CCEDM,     AZ_OEM,       AZ_OCIRM,     AZ_BULLET,                                XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      AZ_UCIRM,   
//-----------+-------------+-------------+=============+-------------+                           +-------------+=============+-------------+-------------+-------------+ 
AZ_AGRVM,     AZ_EACUM,     AZ_EGRVM,     AZ_ECIRM,     AZ_NTILDM,                                XXXXXXX,      XXXXXXX,      AZ_ICIRM,     AZ_ITREM,     AZ_UGRVM,
//-----------+-------------+-------------+=============+-------------+                           +-------------+=============+-------------+-------------+-------------+ 
AZ_AEM,       AZ_SUP3,      XXXXXXX,      XXXXXXX,      XXXXXXX,                                  XXXXXXX,      XXXXXXX,      XXXXXXX,      XXXXXXX,      AZ_RGSTRD,
//-----------+-------------+-------------+-------------+=============+-------------+-------------+=============+-------------+-------------+-------------+-------------+
                                          _______,      XXXXXXX,      TO(_BAS),     TO(_BAS),     XXXXXXX,      XXXXXXX        
//                                       +-------------+=============+-------------+-------------+=============+-------------+     
    ),

    [_COD] = LAYOUT_36keys(
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+
AZ_DQUO,      AZ_LABK,      AZ_RABK,      AZ_AMPR,      AZ_PERC,                                  AZ_DLR,       AZ_EURO,      AZ_SML1,      AZ_SML2,      AZ_SML3,   
//-----------+-------------+-------------+=============+-------------+                           +-------------+=============+-------------+-------------+-------------+ 
AZ_LCBR,      AZ_LPRN,      AZ_RPRN,      AZ_RCBR,      AZ_PIPE,                                  KC_LEFT,      KC_DOWN,      KC_UP,        KC_RGHT,      XXXXXXX,
//-----------+-------------+-------------+=============+-------------+                           +-------------+=============+-------------+-------------+-------------+ 
AZ_QUOT,      AZ_LBRC,      AZ_RBRC,      AZ_UNDS,      AZ_HASH,                                  KC_HOME,      KC_PGDN,      KC_PGUP,      KC_END,       XXXXXXX,
//-----------+-------------+-------------+-------------+=============+-------------+-------------+=============+-------------+-------------+-------------+-------------+
                                          _______,      XXXXXXX,      XXXXXXX,      _______,      KC_LSFT,      _______        
//                                       +-------------+=============+-------------+-------------+=============+-------------+     
    ),

    [_NUM] = LAYOUT_36keys(
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+
KC_TAB,       AZ_AT,        KC_SPC,       AZ_TIL,       KC_ESC,                                   XXXXXXX,      AZ_DOT,       AZ_COMM,      XXXXXXX,      AZ_BSLH,   
//-----------+-------------+-------------+=============+-------------+                           +-------------+=============+-------------+-------------+-------------+ 
AZ_1,         AZ_2,         AZ_3,         AZ_4,         AZ_5,                                     AZ_6,         AZ_7,         AZ_8,         AZ_9,         AZ_0,
//-----------+-------------+-------------+=============+-------------+                           +-------------+=============+-------------+-------------+-------------+ 
A(KC_TAB),    AZ_LPRN,      AZ_RPRN,      AZ_CIR,       AZ_GRV,                                   AZ_EQL,       AZ_PLUS,      AZ_MINS,      AZ_ASTR,      AZ_SLSH,
//-----------+-------------+-------------+-------------+=============+-------------+-------------+=============+-------------+-------------+-------------+-------------+
                                          _______,      KC_LSFT,      MO(_COD),     _______,      _______,      _______        
//                                       +-------------+=============+-------------+-------------+=============-+-------------+     
    ),

    [_FCT] = LAYOUT_36keys(
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+
QK_REBOOT,    AZ_DETECT_OS, LED_UP,       KC_BRIU,      KC_VOLU,                                  KC_F1,        KC_F2,        KC_F3,        KC_F4,        KC_F5,   
//-----------+-------------+-------------+=============+-------------+                           +-------------+=============+-------------+-------------+-------------+ 
QK_BOOTLOADER,AZ_NEXT_EMUL, LED_DOWN,     KC_BRID,      KC_VOLD,                                  KC_F6,        KC_F7,        KC_F8,        KC_F9,        KC_F10,  
//-----------+-------------+-------------+=============+-------------+                           +-------------+=============+-------------+-------------+-------------+ 
TD_SAFE_FLASH,AZ_REPEAT_TOGGLE,HAPT_TGGL,   XXXXXXX,      XXXXXXX,                                  KC_F11,       KC_F12,       XXXXXXX,      XXXXXXX,      KC_PSCR,  
//-----------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
                                          XXXXXXX,      XXXXXXX,      XXXXXXX,      KC_MUTE,      KB_MICOFF,    XXXXXXX        
//                                       +-------------+-------------+-------------+-------------+-------------+-------------+     
    ),

    [_MSE] = LAYOUT_36keys(
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+
_______,      _______,      _______,      _______,      _______,                                  _______,      _______,      _______,      _______,      _______,   
//-----------+-------------+-------------+=============+-------------+                           +-------------+=============+-------------+-------------+-------------+ 
_______,      _______,      _______,      _______,      _______,                                  _______,      _______,      _______,      _______,      _______,
//-----------+-------------+-------------+=============+-------------+                           +-------------+=============+-------------+-------------+-------------+ 
_______,      _______,      _______,      _______,      _______,                                  _______,      _______,      _______,      _______,      _______,
//-----------+-------------+-------------+-------------+=============+-------------+-------------+=============+-------------+-------------+-------------+-------------+
                                          _______,      _______,      _______,      _______,      MS_BTN1,      MS_BTN2        
//                                       +-------------+=============+-------------+-------------+=============+-------------+     
    ),

    [_OTH] = LAYOUT_36keys(
//-----------+-------------+-------------+-------------+-------------+                           +-------------+-------------+-------------+-------------+-------------+
_______,      _______,      _______,      _______,      _______,                                  _______,      _______,      _______,      _______,      XXXXXXX,   
//-----------+-------------+-------------+=============+-------------+                           +-------------+=============+-------------+-------------+-------------+ 
_______,      _______,      _______,      _______,      _______,                                  _______,      _______,      _______,      _______,      LED_FR,
//-----------+-------------+-------------+=============+-------------+                           +-------------+=============+-------------+-------------+-------------+ 
_______,      _______,      _______,      _______,      _______,                                  _______,      _______,      _______,      _______,      LED_EU,
//-----------+-------------+-------------+-------------+=============+-------------+-------------+=============+-------------+-------------+-------------+-------------+
                                          _______,      _______,      _______,      _______,      _______,      _______        
//                                       +-------------+=============+-------------+-------------+=============+-------------+     
    )

};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  tap_dance_action_t *action;

  //release MOUSE layer if any other key is pressed
  if (trackpoint_timer && keycode!=MS_BTN1 && keycode!=MS_BTN2 && keycode!=MS_BTN3 && record->event.pressed) { 
      layer_off(_MSE);
      //haptic_module_pulse_default();
      haptic_module_double_tick();
      //unregister_code(MS_BTN1);
      //unregister_code(MS_BTN2);
      //unregister_code(MS_BTN3);
      trackpoint_timer = 0; //Reset the timer again until the mouse moves more
  }

  // MOUSE btns
  if (keycode == MS_BTN1 || keycode == MS_BTN2) {
    if (record->event.pressed) haptic_module_pulse_default();
    trackpoint_timer = timer_read(); 
    //enable _NUM when MS_BTN1 is hold
    if (keycode == MS_BTN1) {
      if (record->event.pressed) layer_on(_NUM);
      else layer_off(_NUM);
    }
  }

  //KEYS
  switch (keycode) {
    //TAP DANCE section
    case TD(TD2_BSPC_MS3):
      action = &tap_dance_actions[QK_TAP_DANCE_GET_INDEX(keycode)];
      if (!record->event.pressed && action->state.count && !action->state.finished) {
        tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
        tap_code16(tap_hold->tap);
      }
      break;

    //EMUL KEYS
    case AZ_FIRST_EMUL...AZ_LAST_EMUL:
      emul_process_record_user(keycode, record);
      break;
    
    //OTHER KEYS
    case MA_CMD_ALT_TAB:
      if (record->event.pressed) { 
        if (emul_get_os() == EMUL_OS_OSX) SEND_STRING(SS_LGUI(SS_TAP(X_TAB))); else SEND_STRING(SS_LALT(SS_TAP(X_TAB)));
      } break;  

    // FUNCT
    case KB_PSCR:
      if (record->event.pressed) {
        if (emul_get_os() == EMUL_OS_OSX) SEND_STRING(SS_LSFT(SS_LCMD(SS_TAP(X_4)))); else SEND_STRING(SS_TAP(X_PSCR));
      } break;

    case KB_MICOFF: 
      if (record->event.pressed) {
        if (emul_get_os() == EMUL_OS_OSX) SEND_STRING(SS_LCMD(SS_TAP(X_F4))); else SEND_STRING(SS_LWIN(SS_LALT(SS_TAP(X_K))));
      } break;

    case HAPT_TGGL: 
      if (record->event.pressed) {
        bool enabled = haptic_module_toggle();
        if (enabled) leds_on_for_range(500, HS_GREEN, LED_LEFT, LED_LEFT+1); else leds_on_for_range(500, HS_RED, LED_LEFT, LED_LEFT+1);
      } break;

    case HAPT_INC:  if (record->event.pressed) haptic_module_increase(); break;
    case HAPT_DEC:  if (record->event.pressed) haptic_module_decrease(); break;
    
    case LED_UP:    if (record->event.pressed) leds_val_inc(); break;
    case LED_DOWN:  if (record->event.pressed) leds_val_dec(); break;
    
    case LED_FR:    if (record->event.pressed) leds_anim_start(0); else leds_anim_stop(); break;
    case LED_EU:    if (record->event.pressed) leds_anim_start(1); else leds_anim_stop(); break;

    case QK_BOOTLOADER: if (record->event.pressed) { leds_seths_range(HS_ORANGE,0,LED_COUNT); } break;
  }
  return true;
}

// LED for some layers
layer_state_t layer_state_set_user(layer_state_t state) {
    leds_off_range(0, LED_COUNT);
    // LEFT LED
    if (/*layer_state_cmp(state,_SHF) ||*/ layer_state_cmp(state,_SDK)) {
      leds_seths_at(HS_BLUE, LED_LEFT);
    } else {
      leds_off_at(LED_LEFT);
    }

    //BOTTOM LED
    /* if (layer_state_cmp(state,_1DK) || layer_state_cmp(state,_SDK)) {
      leds_seths_at(8,192, LED_BOTTOM);
    } else */ 
    if (layer_state_cmp(state,_COD)) {
      leds_seths_at(HS_GREEN, LED_BOTTOM);
    } else if (layer_state_cmp(state, _NUM)) {
      haptic_module_small_tick();
      leds_seths_at(HS_PURPLE, LED_BOTTOM);
    } else if (layer_state_cmp(state,_FCT)) {
      leds_seths_at(HS_YELLOW, LED_BOTTOM);
      haptic_module_pulse_default();
    } else {
      leds_off_at(LED_BOTTOM);
    }

    // RIGHT LED
    // Dedicated to mouse layer and 1DK
    bool led_right = false;
    // 1DK led
    if (layer_state_cmp(state,_1DK) || layer_state_cmp(state,_SDK)) {
      switch(emul_get_os()) {
        case EMUL_OS_OSX: leds_seths_at(HS_WHITE, LED_RIGHT); break;
        case EMUL_OS_LINUX: leds_seths_at(HS_GREEN, LED_RIGHT); break;
        case EMUL_OS_WIN: leds_seths_at(HS_BLUE, LED_RIGHT); break;
      }
      led_right = true;
    }
    // Mouse layer led
    if (layer_state_cmp(state,_MSE)) {
      leds_seths_at(HS_RED, LED_RIGHT);
      led_right = true;
    }
    // otherwise off
    if (!led_right) leds_off_at(LED_RIGHT);

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
      //haptic_module_pulse_default();
      haptic_module_double_tick();
      trackpoint_timer = 0; //Reset the timer again until the mouse moves more
      trackpoint_lock_dir = TP_FREE;
    }
  }
  emul_matrix_scan_user();
}

//disable haptic for all keys (used only when leaving mouse layer)
bool get_haptic_enabled_key(uint16_t keycode, keyrecord_t *record) {
  return false;
}

