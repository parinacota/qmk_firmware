#pragma once
// Emulate special characters for AZERTY layout
// Usage:
// 0. add these options in rules.mk:
//      OS_DETECTION_ENABLE = yes
//      DEFERRED_EXEC_ENABLE = yes
// 1. Add <emul_azerty.h> to your keymap.c
// 2. Add the following code to your config.h
//      call emul_keyboard_post_init_user() in your keyboard_post_init_user() --> will detect the host OS -- 
//      call process_record_user_emul(keycode, record) in your process_record_user() --> will emulate the keycodes
//      call matrix_scan_user_emul() in your matrix_scan_user() --> needed for the repeatition of the keycodes (if used)
// 3. define your custom keycodes in your keymap.c like this:
//      enum custom_keycodes_emul {
//         MY_FIRST_CUSTOM_KEYCODE = ME_LAST_EMUL, // ME_LAST_EMUL is the last keycode of the emul layer, that starts its counter with SAFE_RANGE
//         ...
//      }
// 4. Use the ME_xxxx keycodes in your keymaps
// 5. optionnaly:
//      call emul_set_os() with the detected OS (result of detected_host_os())
//      call emul_get_os() to get the current OS (EMUL_OS_WIN or EMUL_OS_OSX)
//      use the keycodes ME_REPEAT_TOGGLE, ME_REPEAT_DEC, ME_REPEAT_INC to control key repeat
// 6. Enjoy

#define _EMUL_INITIAL_REPEAT_DELAY 300 
#define _EMUL_REPEAT_DELAY 70
#define _EMUL_REPEAT_DELAY_MIN 30
#define _EMUL_REPEAT_DELAY_MAX 200

uint16_t _emul_initial_repeat_delay = _EMUL_INITIAL_REPEAT_DELAY;
uint16_t _emul_repeat_delay = _EMUL_REPEAT_DELAY;
bool _emul_repeat_active = false;
bool _emul_repeat_enabled = false;
uint16_t _emul_repeat_timer = 0;
uint16_t _emul_delay;
uint16_t _emul_last_keycode = 0;

typedef enum {
    EMUL_OS_WIN,
    EMUL_OS_OSX
} emul_os_types;

emul_os_types _emul_os_mode = EMUL_OS_WIN;

void emul_set_os(os_variant_t os) {
    switch(os) {
        case OS_MACOS:
        case OS_IOS:
            _emul_os_mode = EMUL_OS_OSX;
            break;
        default:
            _emul_os_mode = EMUL_OS_WIN;
    }
}

emul_os_types emul_get_os(void) {
    return _emul_os_mode;
}

bool emul_toggle_repeat(void) {
    _emul_repeat_enabled = !_emul_repeat_enabled;
    return _emul_repeat_enabled;
}

uint32_t _emul_get_host_os(uint32_t trigger_time, void *cb_arg) {
    emul_set_os(detected_host_os());
    return 0;
}

typedef struct {
    uint16_t kc1;
    uint16_t kc2;
    uint16_t kc_held;
} _emul_tap_dance_pair_invert_t;

void _emul_tap_dance_pair_invert_on_each_tap(tap_dance_state_t *state, void *user_data) {
    _emul_tap_dance_pair_invert_t *pair = (_emul_tap_dance_pair_invert_t *)user_data;
    if (state->count == 2) {
        pair->kc_held = (emul_get_os() == EMUL_OS_OSX ? pair->kc1 : pair->kc2);
        register_code16(pair->kc_held);
        state->finished = true;
    }
}

void _emul_tap_dance_pair_invert_finished(tap_dance_state_t *state, void *user_data) {
    _emul_tap_dance_pair_invert_t *pair = (_emul_tap_dance_pair_invert_t *)user_data;
    pair->kc_held = (emul_get_os() == EMUL_OS_OSX ? pair->kc2 : pair->kc1);
    register_code16(pair->kc_held);
}

void _emul_tap_dance_pair_invert_reset(tap_dance_state_t *state, void *user_data) {
    _emul_tap_dance_pair_invert_t *pair = (_emul_tap_dance_pair_invert_t *)user_data;
    if (state->count == 1) wait_ms(TAP_CODE_DELAY);
    unregister_code16(pair->kc_held);
}

#define EMUL_ACTION_TAP_DANCE_DOUBLE_INVERT_OSX(kc1, kc2) \
    { .fn = {_emul_tap_dance_pair_invert_on_each_tap, _emul_tap_dance_pair_invert_finished, _emul_tap_dance_pair_invert_reset, NULL}, .user_data = (void *)&((_emul_tap_dance_pair_invert_t){kc1, kc2}), }

void emul_keyboard_post_init_user(void) {
    defer_exec(500, _emul_get_host_os, NULL);
}

enum custom_keycodes_emul {
    /*emul BASE LAYER*/ 
    ME_FIRST_EMUL = SAFE_RANGE, 
    ME_EURO, 
    ME_AT,  
    ME_MINS, 

    /*emul SHIFT LAYER*/ 
    ME_EXCL, 
    ME_INSEC, 
    ME_DQUO, 
    ME_SCLN, 
    ME_COLN, 
    ME_DEL, 
    ME_BSLH, 
    ME_DLR1, 

    /*emul DEAD LAYER*/ 
    ME_ACIR, 
    ME_OE, 
    ME_OCIR, 
    ME_UCIR, 
    ME_ECIR, 
    ME_ICIR, 
    ME_ITRE, 
    ME_AE, 
    ME_ETC, 
    ME_QUOTFR, 
    ME_GRV, 
    ME_CADR, 
    ME_SCADR, 
    ME_QCADR, 
    ME_LQUOTFR, 
    ME_RQUOTFR, 
    ME_UNDS, 
    ME_DLR, 
    ME_NTILD, 
    ME_MICR, 
    ME_SUP2, 
    ME_SECT, 
    ME_COPYR, 
    ME_BULLET, 
    ME_https, 

    /*emul DEAD MAJ LAYER*/ 
    ME_ACIRM, 
    ME_CCEDM, 
    ME_OEM, 
    ME_OCIRM, 
    ME_UCIRM, 
    ME_AGRVM, 
    ME_EACUM, 
    ME_EGRVM, 
    ME_ECIRM, 
    ME_ICIRM, 
    ME_ITREM, 
    ME_UGRVM, 
    ME_AEM, 
    ME_SUP3, 
    ME_NTILDM, 
    ME_RGSTRD, 

    /*emul CODE LAYER*/ 
    ME_CIR, 
    ME_TIL, 
    ME_LCBR, 
    ME_RCBR, 
    ME_LBRC, 
    ME_RBRC, 
    ME_PLUS, 
    ME_ASTR, 
    ME_EQL, 
    ME_PIPE, 
    ME_LABK, 
    ME_RABK, 
    ME_HASH, 
    ME_SML1, 
    ME_SML2, 
    ME_SML3,
    
    ME_COMMANDS,
    ME_REPEAT_TOGGLE,
    ME_REPEAT_DEC,
    ME_REPEAT_INC,
    ME_LAST_EMUL
};

#define UNMOD(A) {uint8_t _emul_mod_state = get_mods(); clear_mods(); A; set_mods(_emul_mod_state);}
#define WINALT1(A) {SEND_STRING(SS_LALT(SS_TAP(X_KP_ ## A)));}
#define WINALT2(A, B) {SEND_STRING(SS_LALT(SS_TAP(X_KP_ ## A) SS_TAP(X_KP_ ## B)));}
#define WINALT3(A, B, C) {SEND_STRING(SS_LALT(SS_TAP(X_KP_ ## A) SS_TAP(X_KP_ ## B) SS_TAP(X_KP_ ## C)));}
#define WINALT4(A, B, C, D) {SEND_STRING(SS_LALT(SS_TAP(X_KP_ ## A) SS_TAP(X_KP_ ## B) SS_TAP(X_KP_ ## C) SS_TAP(X_KP_ ## D)));}
#define SHIFT(A) {register_code(KC_LSFT); A ; unregister_code(KC_LSFT);}
#define ALTGR(A) {register_code(KC_RALT); A ; unregister_code(KC_RALT);}
#define OPTION(A) {register_code(KC_LOPT); A ; unregister_code(KC_LOPT);}
#define COMMAND(A) {register_code(KC_LGUI); A ; unregister_code(KC_LGUI);}
#define XKEY(A) {SEND_STRING(SS_TAP(A));}
#define XCIRC(A) {SEND_STRING(SS_TAP(X_LBRC) SS_TAP(A));}
#define XCIRCSHIFT(A) {SEND_STRING(SS_TAP(X_LBRC) SS_LSFT(SS_TAP(A)));}
#define XACUTSHIFT(A) {SEND_STRING(SS_LSFT(SS_LALT(SS_TAP(X_1))) SS_LSFT(SS_TAP(A)));}
#define XTRE(A) {SEND_STRING(SS_LSFT(SS_TAP(X_LBRC)) SS_TAP(A));}
#define XGRV(A) {SEND_STRING(SS_TAP(X_NUHS) SS_TAP(A));}
#define XGRVSHIFT(A) {SEND_STRING(SS_TAP(X_NUHS) SS_LSFT(SS_TAP(A)));}
#define XTILD(A) {SEND_STRING(SS_LALT(SS_TAP(X_N)) SS_TAP(A));}
#define XTILDSHIFT(A) {SEND_STRING(SS_LALT(SS_TAP(X_N)) SS_LSFT(SS_TAP(A)));}

void _emul_send_key(uint16_t keycode) {
    switch (keycode) {
        //ERGO BASE LAYER
        case ME_MINS: if (_emul_os_mode == EMUL_OS_OSX)      XKEY(X_EQL)                  else XKEY(X_6);               break;
        //emul SHIFT LAYER
        case ME_EXCL: if (_emul_os_mode == EMUL_OS_OSX)      UNMOD(XKEY(X_8))           else UNMOD(XKEY(X_SLSH));   break;
        case ME_INSEC:if (_emul_os_mode == EMUL_OS_OSX)      UNMOD(OPTION(XKEY(X_SPC))) else UNMOD(WINALT3(2,5,5)); break; // ALT 255 = insec
        case ME_SCLN: UNMOD(XKEY(X_COMM)); break; 
        case ME_COLN: UNMOD(XKEY(X_DOT)); break;
        case ME_DEL:  UNMOD(XKEY(X_DEL)); break;
        // emul DEAD LAYER
        case ME_ACIR:   if (_emul_os_mode == EMUL_OS_OSX) XCIRC(X_Q)                else WINALT3(1,3,1); break;// ALT 131 = â
        case ME_OE:     if (_emul_os_mode == EMUL_OS_OSX) OPTION(XKEY(X_O))         else WINALT4(0,1,5,6); break;//ALT 0156 = œ
        case ME_OCIR:   if (_emul_os_mode == EMUL_OS_OSX) XCIRC(X_O)                else WINALT3(1,4,7); break;// ALT 147 = ô
        case ME_UCIR:   if (_emul_os_mode == EMUL_OS_OSX) XCIRC(X_U)                else WINALT3(1,5,0); break;// ALT 150 = û
        case ME_ECIR:   if (_emul_os_mode == EMUL_OS_OSX) OPTION(XKEY(X_E))         else WINALT3(1,3,6); break;// ALT 136 = ê
        case ME_ICIR:   if (_emul_os_mode == EMUL_OS_OSX) OPTION(XKEY(X_I))         else WINALT3(1,4,0); break;// ALT 140 = î
        case ME_ITRE:   if (_emul_os_mode == EMUL_OS_OSX) XTRE(X_I)                 else WINALT3(1,3,9); break;// ALT 139 = ï
        case ME_AE:     if (_emul_os_mode == EMUL_OS_OSX) OPTION(XKEY(X_Q))         else WINALT3(1,4,5); break;// ALT 145 = æ
        case ME_ETC:    if (_emul_os_mode == EMUL_OS_OSX) OPTION(XKEY(X_COMM))      else WINALT4(0,1,3,3); break; // ...
        case ME_QUOTFR: if (_emul_os_mode == EMUL_OS_OSX) SHIFT(OPTION(XKEY(X_4)))  else WINALT4(0,1,4,6); break;//ALT 0146 = ’
        case ME_CADR:   if (_emul_os_mode == EMUL_OS_OSX) OPTION(XKEY(X_EQL))       else WINALT4(0,1,5,1); break;// ALT 151 = 
        case ME_SCADR:  if (_emul_os_mode == EMUL_OS_OSX) SHIFT(OPTION(XKEY(X_EQL)))else WINALT4(0,1,5,0); break;// ALT 0150 = 
        case ME_QCADR:  if (_emul_os_mode == EMUL_OS_OSX) XKEY(X_EQL)               else WINALT4(0,1,7,3); break;// ALT 0173 = - (insec)
        case ME_UNDS:   if (_emul_os_mode == EMUL_OS_OSX)  SHIFT(XKEY(X_EQL))       else XKEY(X_8); break; // _
        case ME_LQUOTFR: if (_emul_os_mode == EMUL_OS_OSX) OPTION(XKEY(X_7))        else WINALT3(1,7,4); break;// ALT 174 = «
        case ME_RQUOTFR: if (_emul_os_mode == EMUL_OS_OSX) SHIFT(OPTION(XKEY(X_7))) else WINALT3(1,7,5); break;// ALT 175 = »
        case ME_NTILD:  if (_emul_os_mode == EMUL_OS_OSX) XTILD(X_N)                else WINALT3(1,6,4); break;// ALT 164 = ñ
        case ME_MICR:   if (_emul_os_mode == EMUL_OS_OSX) OPTION(XKEY(X_SCLN))      else SHIFT(XKEY(X_NUHS)); break;// ALT 181 = µ
        case ME_SUP2:   if (_emul_os_mode == EMUL_OS_WIN)                               XKEY(X_GRV); break;// ALT 178 = ²
        case ME_SECT:   if (_emul_os_mode == EMUL_OS_OSX)  XKEY(X_6)                else SHIFT(XKEY(X_SLSH)); break;// ALT 167 = §
        case ME_COPYR:  if (_emul_os_mode == EMUL_OS_OSX)  OPTION(XKEY(X_C))        else WINALT4(0,1,6,9); break;// ALT 0169 = ©
        case ME_https:  SEND_STRING("https" SS_TAP(X_DOT) SS_LSFT(SS_TAP(X_DOT)) SS_LSFT(SS_TAP(X_DOT))); break; // https://
        //emul DEAD MAJ LAYER
        case ME_ACIRM: if (_emul_os_mode == EMUL_OS_OSX)   XCIRCSHIFT(X_Q)          else WINALT3(1,8,2); break;// ALT 182 = Â
        case ME_CCEDM: if (_emul_os_mode == EMUL_OS_OSX)   OPTION(XKEY(X_9))        else WINALT3(1,2,8); break;// ALT 128 = Ç
        case ME_OEM: if (_emul_os_mode == EMUL_OS_OSX)     SHIFT(OPTION(XKEY(X_O))) else WINALT4(0,1,4,0); break;// ALT 0140 = Œ
        case ME_OCIRM: if (_emul_os_mode == EMUL_OS_OSX)   SHIFT(OPTION(XKEY(X_LBRC))) else WINALT3(2,2,6); break;// ALT 226 = Ô
        case ME_UCIRM: if (_emul_os_mode == EMUL_OS_OSX)   SHIFT(OPTION(XKEY(X_8))) else WINALT3(2,3,4); break;// ALT 234 = Û
        case ME_AGRVM: if (_emul_os_mode == EMUL_OS_OSX)   SEND_STRING(SS_TAP(X_CAPS) SS_TAP(X_0) SS_TAP(X_CAPS)); else WINALT3(1,8,3); break;//ALT 183 = À
        case ME_EACUM: if (_emul_os_mode == EMUL_OS_OSX)   XACUTSHIFT(X_E)          else WINALT3(1,4,4); break;//ALT 144 = É
        case ME_EGRVM: if (_emul_os_mode == EMUL_OS_OSX)   XGRVSHIFT(X_E)           else WINALT3(2,1,2); break;//ALT 212 = È
        case ME_ECIRM: if (_emul_os_mode == EMUL_OS_OSX)   SHIFT(OPTION(XKEY(X_E))) else WINALT3(2,1,0); break;// ALT 210 = Ê
        case ME_ICIRM: if (_emul_os_mode == EMUL_OS_OSX)   SHIFT(OPTION(XKEY(X_H))) else WINALT3(2,1,5); break;// ALT 215 = Î
        case ME_ITREM: if (_emul_os_mode == EMUL_OS_OSX)   OPTION(XKEY(X_J))        else WINALT3(2,1,6); break;// ALT 216 = Ï
        case ME_UGRVM: if (_emul_os_mode == EMUL_OS_OSX)   OPTION(XKEY(X_QUOT))     else WINALT3(2,3,5); break;// ALT 235 = Ù
        case ME_AEM:   if (_emul_os_mode == EMUL_OS_OSX)   SHIFT(OPTION(XKEY(X_Q))) else WINALT3(1,4,6); break;// ALT 146 = Æ
        case ME_SUP3:  if (_emul_os_mode == EMUL_OS_WIN)                                WINALT3(2,5,2); break;// ALT 252 = ³
        case ME_NTILDM: if (_emul_os_mode == EMUL_OS_OSX)  XTILDSHIFT(X_N)          else WINALT3(1,6,5); break;// ALT 165 = Ñ
        case ME_RGSTRD: if (_emul_os_mode == EMUL_OS_OSX)  OPTION(XKEY(X_R))        else WINALT4(0,1,7,4); break; // ALT 0174 = ®
        case ME_BULLET: if (_emul_os_mode == EMUL_OS_OSX)  OPTION(XKEY(X_NUBS))     else WINALT1(7); break;// ALT 7 = •
        //emul CODE LAYER
        case ME_EURO: if (_emul_os_mode == EMUL_OS_OSX)   OPTION(XKEY(X_RBRC))      else ALTGR(XKEY(X_E));        break;
        case ME_LCBR:  if (_emul_os_mode == EMUL_OS_OSX)  OPTION(XKEY(X_5))         else ALTGR(XKEY(X_4)); break; // {
        case ME_RCBR:  if (_emul_os_mode == EMUL_OS_OSX)  OPTION(XKEY(X_MINS))      else ALTGR(XKEY(X_EQL)); break; // {
        case ME_LBRC:  if (_emul_os_mode == EMUL_OS_OSX)  SHIFT(OPTION(XKEY(X_5)))  else ALTGR(XKEY(X_5)); break; // [
        case ME_RBRC:  if (_emul_os_mode == EMUL_OS_OSX)  SHIFT(OPTION(XKEY(X_MINS))) else ALTGR(XKEY(X_MINS)); break; // ]
        case ME_PLUS:  if (_emul_os_mode == EMUL_OS_OSX)  SHIFT(XKEY(X_SLSH))       else SHIFT(XKEY(X_EQL)); break; // +
        case ME_EQL:   if (_emul_os_mode == EMUL_OS_OSX)  XKEY(X_SLSH)              else XKEY(X_EQL); break; // =
        case ME_ASTR:  if (_emul_os_mode == EMUL_OS_OSX)  SHIFT(XKEY(X_RBRC))       else XKEY(X_NUHS); break; // *
        case ME_PIPE:  if (_emul_os_mode == EMUL_OS_OSX)  SHIFT(OPTION(XKEY(X_L)))  else ALTGR(XKEY(X_6)); break; // |
        case ME_LABK:  if (_emul_os_mode == EMUL_OS_OSX)  XKEY(X_GRV)               else XKEY(X_NUBS); break; // <
        case ME_RABK:  if (_emul_os_mode == EMUL_OS_OSX)  SHIFT(XKEY(X_GRV))        else SHIFT(XKEY(X_NUBS)); break; // >
        case ME_HASH:  if (_emul_os_mode == EMUL_OS_OSX)  SHIFT(XKEY(X_NUBS))       else ALTGR(XKEY(X_3)); break; // #
        case ME_SML1:  if (_emul_os_mode == EMUL_OS_OSX)  SEND_STRING(SS_TAP(X_DOT)  SS_TAP(X_EQL) SS_TAP(X_MINS)); else SEND_STRING(SS_TAP(X_DOT)  SS_TAP(X_6) SS_TAP(X_MINS)); break;
        case ME_SML2:  if (_emul_os_mode == EMUL_OS_OSX)  SEND_STRING(SS_TAP(X_COMM) SS_TAP(X_EQL) SS_TAP(X_MINS)); else SEND_STRING(SS_TAP(X_COMM) SS_TAP(X_6) SS_TAP(X_MINS)); break;
        case ME_SML3:  if (_emul_os_mode == EMUL_OS_OSX)  SEND_STRING(SS_TAP(X_DOT)  SS_TAP(X_EQL) SS_TAP(X_5));    else SEND_STRING(SS_TAP(X_DOT)  SS_TAP(X_6) SS_TAP(X_5)); break;
        // emul NUM
        case ME_BSLH: if (_emul_os_mode == EMUL_OS_OSX)      SHIFT(OPTION(XKEY(X_DOT)))    else ALTGR(XKEY(X_8)); break;
        case ME_AT:   if (_emul_os_mode == EMUL_OS_OSX)      XKEY(X_NUBS)                 else ALTGR(XKEY(X_0));        break;
        case ME_GRV:    if (_emul_os_mode == EMUL_OS_OSX) XGRV(X_SPC)               else WINALT2(9,6); break; // ALT 96 = ``
        case ME_CIR:   if (_emul_os_mode == EMUL_OS_OSX)  XCIRC(X_SPC)              else WINALT2(9,4); break; // ALT 94 = ^
        case ME_TIL:   if (_emul_os_mode == EMUL_OS_OSX)  XTILD(X_SPC)              else WINALT3(1,2,6); break; // ALT 126 = ~
        /* case MA_EOL: WINALT2(2,0); break; // ALT 20 = ¶  */
    } // switch
}

bool process_record_user_emul(uint16_t keycode, keyrecord_t *record) {
    if (keycode > ME_FIRST_EMUL && keycode < ME_LAST_EMUL  && record->event.pressed) {
        if (keycode > ME_COMMANDS) {
            switch(keycode) {
                case ME_REPEAT_TOGGLE: emul_toggle_repeat(); break;
                case ME_REPEAT_DEC: _emul_repeat_delay = (_emul_repeat_delay > _EMUL_REPEAT_DELAY_MIN) ? _emul_repeat_delay - 10 : _EMUL_REPEAT_DELAY_MIN; break;
                case ME_REPEAT_INC: _emul_repeat_delay = (_emul_repeat_delay < _EMUL_REPEAT_DELAY_MAX) ? _emul_repeat_delay + 10 : _EMUL_REPEAT_DELAY_MAX; break;
            }
        } else {
            _emul_send_key(keycode);
            if (_emul_repeat_enabled) {
                _emul_last_keycode = keycode;
                _emul_repeat_timer = timer_read(); // Reset spam timer
                _emul_delay = _emul_initial_repeat_delay;
                _emul_repeat_active = true;
            }
        }
    } else {
        //released, or other key
        _emul_repeat_active = false;
    }
    return true;
}

void matrix_scan_user_emul(void) {
    if (_emul_repeat_active && timer_elapsed(_emul_repeat_timer) > _emul_delay) {
        _emul_send_key(_emul_last_keycode);
        _emul_repeat_timer = timer_read();
        _emul_delay = _emul_repeat_delay;
    }
}