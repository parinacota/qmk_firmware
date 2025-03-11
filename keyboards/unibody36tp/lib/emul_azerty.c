#include QMK_KEYBOARD_H 
#include "emul_azerty.h"
#include "keymap_french.h"

// ****************************************************
// See emul_azerty.h for the definition of the keycodes
// ****************************************************

uint16_t _emul_initial_repeat_delay = _EMUL_INITIAL_REPEAT_DELAY;
uint16_t _emul_repeat_delay = _EMUL_REPEAT_DELAY;
bool _emul_repeat_active = false;
bool _emul_repeat_enabled = false;
uint16_t _emul_repeat_timer = 0;
uint16_t _emul_delay;
uint16_t _emul_last_keycode = 0;

emul_os_types _emul_os_mode = EMUL_OS_WIN;

// Callback definition to notify the user of the OS change
// This function should be implemented in the keymap
__attribute__((weak)) void emul_notify_event_callback(emul_event_t ev) {}

void emul_set_os(emul_os_types os) {
    switch(os) {
        case EMUL_OS_OSX:
            _emul_os_mode = EMUL_OS_OSX;
            _emul_repeat_enabled = false;
            emul_notify_event_callback(EMUL_EVENT_OS_OSX);
            break;
        case EMUL_OS_LINUX:
            _emul_os_mode = EMUL_OS_LINUX;
            _emul_repeat_enabled = true;
            emul_notify_event_callback(EMUL_EVENT_OS_LINUX);
            break;
        default:
            _emul_os_mode = EMUL_OS_WIN;
            _emul_repeat_enabled = true;
            emul_notify_event_callback(EMUL_EVENT_OS_WIN);
    }
}

void emul_set_next_emul(void) {
    _emul_os_mode = (_emul_os_mode + 1) % 3;
    emul_set_os(_emul_os_mode);
}

emul_os_types emul_get_os(void) {
    return _emul_os_mode;
}

bool emul_toggle_repeat(void) {
    _emul_repeat_enabled = !_emul_repeat_enabled;
    emul_notify_event_callback((_emul_repeat_enabled ? EMUL_EVENT_REPEAT_ON : EMUL_EVENT_REPEAT_OFF));
    return _emul_repeat_enabled;
}

emul_os_types emul_detect_os(void) {
    os_variant_t os = detected_host_os();
    switch(os) {
        case OS_MACOS:
        case OS_IOS:
            emul_set_os(EMUL_OS_OSX);
            break;
        case OS_LINUX: // BUG in WMK, Windows is reporter as a Linux...
            emul_set_os(EMUL_OS_WIN);
            break;
        default:
            emul_set_os(EMUL_OS_WIN);
    }
    return _emul_os_mode;
}

uint32_t _emul_get_host_os(uint32_t trigger_time, void *cb_arg) {
    emul_detect_os();
    return 0;
}

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

void emul_keyboard_post_init_user(void) {
    defer_exec(500, _emul_get_host_os, NULL);
}

#define UNMOD(A) {\
    uint8_t _emul_mod_state = get_mods();\
    clear_mods();\
    A;\
    set_mods(_emul_mod_state);}

#define WINALT1(A) {\
    uint8_t _emul_mod_state = get_mods();\
    clear_mods(); \
    SEND_STRING(SS_LALT(SS_TAP(X_KP_ ## A)));\
    set_mods(_emul_mod_state);}

#define WINALT2(A, B) {\
    uint8_t _emul_mod_state = get_mods();\
    clear_mods(); \
    SEND_STRING(SS_LALT(SS_TAP(X_KP_ ## A) SS_TAP(X_KP_ ## B)));\
    set_mods(_emul_mod_state);}

#define WINALT3(A, B, C) {\
    uint8_t _emul_mod_state = get_mods();\
    clear_mods(); \
    SEND_STRING(SS_LALT(SS_TAP(X_KP_ ## A) SS_TAP(X_KP_ ## B) SS_TAP(X_KP_ ## C)));\
    set_mods(_emul_mod_state);}

#define WINALT4(A, B, C, D) {\
    uint8_t _emul_mod_state = get_mods();\
    clear_mods(); \
    SEND_STRING(SS_LALT(SS_TAP(X_KP_ ## A) SS_TAP(X_KP_ ## B) SS_TAP(X_KP_ ## C) SS_TAP(X_KP_ ## D)));\
    set_mods(_emul_mod_state);}

#define SHIFT(A) {\
    register_code(KC_LSFT);\
    A;\
    unregister_code(KC_LSFT);}

#define ALTGR(A) {\
    register_code(KC_RALT);\
    A;\
    unregister_code(KC_RALT);}

#define OPTION(A) {\
    register_code(KC_LOPT);\
    A;\
    unregister_code(KC_LOPT);}

#define COMMAND(A) {\
    register_code(KC_LCMD);\
    A;\
    unregister_code(KC_LCMD);}

#define UNSHIFT(A) {\
    uint8_t _shifted = get_mods() & MOD_MASK_SHIFT;\
    if (_shifted) unregister_code(KC_LSFT);\
    A;\
    if (_shifted) register_code(KC_LSFT);}

#define XKEY(A) {\
    SEND_STRING(SS_TAP(A));}

#define XCIRC(A) {\
    uint8_t _emul_mod_state = get_mods();\
    clear_mods(); \
    SEND_STRING(SS_TAP(X_LBRC) SS_TAP(A));\
    set_mods(_emul_mod_state);}

#define XCIRCSHIFT(A) {\
    uint8_t _emul_mod_state = get_mods();\
    clear_mods(); \
    SEND_STRING(SS_TAP(X_LBRC) SS_LSFT(SS_TAP(A)));\
    set_mods(_emul_mod_state);}

#define XACUTSHIFT(A) {\
    uint8_t _emul_mod_state = get_mods();\
    clear_mods(); \
    SEND_STRING(SS_LSFT(SS_LALT(SS_TAP(X_1))) SS_LSFT(SS_TAP(A)));\
    set_mods(_emul_mod_state);}

#define XTRE(A) {\
    uint8_t _emul_mod_state = get_mods();\
    clear_mods(); \
    SEND_STRING(SS_LSFT(SS_TAP(X_LBRC)) SS_TAP(A));\
    set_mods(_emul_mod_state);}

#define XGRV(A) {\
    uint8_t _emul_mod_state = get_mods();\
    clear_mods(); \
    SEND_STRING(SS_TAP(X_NUHS) SS_TAP(A));\
    set_mods(_emul_mod_state);}

#define XGRVSHIFT(A) {\
    uint8_t _emul_mod_state = get_mods();\
    clear_mods(); \
    SEND_STRING(SS_TAP(X_NUHS) SS_LSFT(SS_TAP(A)));\
    set_mods(_emul_mod_state);}

#define XTILD(A) {\
    uint8_t _emul_mod_state = get_mods();\
    clear_mods(); \
    SEND_STRING(SS_LALT(SS_TAP(X_N)) SS_TAP(A));\
    set_mods(_emul_mod_state);}

#define XTILDSHIFT(A) {\
    uint8_t _emul_mod_state = get_mods();\
    clear_mods(); \
    SEND_STRING(SS_LALT(SS_TAP(X_N)) SS_LSFT(SS_TAP(A)));\
    set_mods(_emul_mod_state);}

#define LIN_UTF(U) {unicode_input_start(); register_hex(U); unicode_input_finish();}

void _emul_send_key(uint16_t keycode) {
    switch (keycode) {
        //ERGO BASE LAYER
        case AZ_MINS: 
            switch (_emul_os_mode) { 
                case EMUL_OS_OSX: XKEY(X_EQL); break;            //OSX: TAP = - on = key  
                default: UNSHIFT(XKEY(X_6)); break;                       //Win, Lin: TAP = - on 6 key
            } break;

        //emul SHIFT LAYER
        case AZ_EXCL:
            switch(_emul_os_mode) {
                case EMUL_OS_OSX: UNSHIFT(XKEY(X_8)); break;      //OSX: TAP = ! on 8 key
                default: UNSHIFT(XKEY(X_SLSH)); break;            //Win, Lin: TAP = ! on / key
            } break;

        case AZ_INSEC:
            switch(_emul_os_mode) {
                case EMUL_OS_OSX: UNSHIFT(OPTION(XKEY(X_SPC))); break; //OSX: TAP = insecable space on CMD+space key
                case EMUL_OS_LINUX: LIN_UTF(0x00A0); break;          //Lin: UTF-00A0 = insecable space
                case EMUL_OS_WIN: WINALT3(2,5,5); break;      //Win: ALT 255 = insec
            } break;

        case AZ_SCLN: 
            UNSHIFT(XKEY(X_COMM)); 
            break; 

        case AZ_COLN:
            UNSHIFT(XKEY(X_DOT)); 
            break;

        case AZ_DEL:
            UNSHIFT(XKEY(X_DEL)); 
            break;

        // emul DEAD LAYER
        case AZ_ACIR:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT3(1,3,1); break;     //Win: ALT 131 = â
                default: XCIRC(X_Q); break;                  //OSX, Lin: TAP = â with dead ^ + Q key
            } break;

        case AZ_OE:     
            switch (_emul_os_mode) { 
                case EMUL_OS_OSX: OPTION(XKEY(X_O)); break;      //OSX: OPTION+O = œ   
                case EMUL_OS_LINUX: LIN_UTF(0x0153); break;      //Lin: UTF-0153 = œ
                case EMUL_OS_WIN: WINALT4(0,1,5,6); break;       //Win: ALT+0156 = œ
            } break;
        
        case AZ_OCIR:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT3(1,4,7); break;        //Win: ALT 147 = ô
                default: XCIRC(X_O); break;                     //OSX: TAP = ô with dead ^ + O key
            } break;

        case AZ_UCIR:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT3(1,5,0); break;       //Win: ALT 150 = û
                default: XCIRC(X_U); break;                    //Lin, OSX: TAP = û with dead ^ + U key
            } break;

        case AZ_ECIR:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT3(1,3,6); break;       //Win: ALT 136 = ê
                case EMUL_OS_OSX: OPTION(XKEY(X_E)) break;     //OSX: OPTION+E = ê
                default: XCIRC(X_E); break;                    //Lin, TAP = ê with dead ^ + E key
            } break;

        case AZ_ICIR:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT3(1,4,0); break;       //Win: ALT 140 = î
                case EMUL_OS_OSX: OPTION(XKEY(X_I)) break;     //OSX: OPTION+I = î
                default: XCIRC(X_I); break;                    //Lin: dead ^ + I
            } break;

        case AZ_ITRE:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT3(1,3,9); break;       //Win: ALT 139 = ï
                default: XTRE(X_U); break;                    //Lin, OSX: TAP = ï with dead ¨ + I key
            } break;

        case AZ_AE:
            switch (_emul_os_mode) { 
                case EMUL_OS_OSX: OPTION(XKEY(X_Q)) break;       //OSX: OPTION+A = œ   
                case EMUL_OS_LINUX: LIN_UTF(0x00E6); break;      //Lin: UTF-00E6 = æ
                case EMUL_OS_WIN: WINALT3(1,4,5); break;         //Win: ALT+145 = æ
            } break;

        case AZ_ETC:
            switch (_emul_os_mode) { 
                case EMUL_OS_OSX: OPTION(XKEY(X_COMM)) break;   //OSX: OPTION+; = ...
                case EMUL_OS_LINUX: LIN_UTF(0x2026); break;     //Lin: UTF-2026 = ...
                case EMUL_OS_WIN: WINALT4(0,1,3,3); break;       //Win: ALT+0133 = ...
            } break;

        case AZ_QUOTFR:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT4(0,1,4,6); break;      //Win: ALT 0146 = ’
                case EMUL_OS_OSX: SHIFT(OPTION(XKEY(X_4))); break; //OSX: SHIFT+OPTION+4 = ’
                case EMUL_OS_LINUX: LIN_UTF(0x2019); break;     //Lin: UTF-2019 = ’
            } break;

        case AZ_CADR:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT4(0,1,5,1); break;       //Win: ALT 0151 = -
                case EMUL_OS_LINUX: LIN_UTF(0x2014); break;      //Lin: UTF-2012 = –
                case EMUL_OS_OSX: OPTION(XKEY(X_EQL)); break;    //OSX: Shift+Option+- = –
            } break;

        case AZ_SCADR: 
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT4(0,1,5,0); break;       //Win: ALT 0150 = -
                case EMUL_OS_LINUX: LIN_UTF(0x2013); break;      //Lin: UTF-2013 = —
                case EMUL_OS_OSX: SHIFT(OPTION(XKEY(X_EQL))); break; //OSX: Shift+Option+- = —
            } break;

        case AZ_QCADR:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT4(0,1,7,3); break;       //Win: ALT 0173 = - (insec)
                case EMUL_OS_LINUX: LIN_UTF(0x00AD); break;      //Lin: UTF-00AD = - (insec)
                case EMUL_OS_OSX: XKEY(X_EQL); break;            //OSX: TAP = - (non insec)
            } break;

        case AZ_UNDS:
            switch(_emul_os_mode) {
                case EMUL_OS_OSX: SHIFT(XKEY(X_EQL)); break;   //OSX: SHIFT+TAP = _ (on EQL key)
                default: XKEY(X_8); break;                     //Win, Lin: TAP = _ (on 8 key)
            } break;

        case AZ_LQUOTFR:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT3(1,7,4); break;       //Win: ALT 174 = «
                case EMUL_OS_LINUX: LIN_UTF(0x00AB); break;    //Lin: UTF-00AB = «
                case EMUL_OS_OSX: OPTION(XKEY(X_7)); break;    //OSX: opt+7 = «
            } break;

        case AZ_RQUOTFR:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT3(1,7,5); break;       //Win: ALT 175 = »
                case EMUL_OS_LINUX: LIN_UTF(0x00BB); break;    //Lin: UTF-00BB = »
                case EMUL_OS_OSX: SHIFT(OPTION(XKEY(X_7))); break; //OSX: SHIFT+opt+7 = »
            } break;

        case AZ_NTILD:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT3(1,6,4); break;       //Win: ALT 164 = ñ
                case EMUL_OS_LINUX: LIN_UTF(0x00F1); break;    //Lin: UTF-00F1 = ñ
                case EMUL_OS_OSX: XTILD(X_N); break;           //OSX: TAP = ñ with dead ~ + N key
            } break;

        case AZ_MICR: 
            switch(_emul_os_mode) {
                case EMUL_OS_OSX: OPTION(XKEY(X_SCLN)); break; //OSX: OPTION+; = µ
                default: SHIFT(XKEY(X_NUHS)); break;           //other: key µ
            } break;

        case AZ_SUP2:
            switch(_emul_os_mode) {
                case EMUL_OS_OSX: /* ??? */ break;            //OSX: Not implemeted
                default: XKEY(X_GRV); break;                  //Win, Lin: TAP ²
            } break;

        case AZ_SECT:
            switch(_emul_os_mode) {
                case EMUL_OS_OSX: XKEY(X_6); break;            //OSX: TAP = § on 6 key
                default: SHIFT(XKEY(X_SLSH)); break;           //Win, Lin: TAP = § on / key
            } break;

        case AZ_COPYR:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT4(0,1,6,9); break;      //Win: ALT 0169 = ©
                case EMUL_OS_LINUX: LIN_UTF(0x00A9); break;     //Lin: UTF-00A9 = ©
                case EMUL_OS_OSX: OPTION(XKEY(X_C)); break;     //OSX: TAP = © with OPTION+C key
            } break;

        case AZ_https: 
            UNMOD(SEND_STRING("https" SS_TAP(X_DOT) SS_LSFT(SS_TAP(X_DOT)) SS_LSFT(SS_TAP(X_DOT))));
            break; // https://

        //emul DEAD MAJ LAYER
        case AZ_ACIRM:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT3(1,8,2); break;                //Win: ALT 182 = Â
                default: XCIRCSHIFT(X_Q); break;                        //Lin, OSX: TAP = Â with dead ^ + Q key
            } break;

        case AZ_CCEDM:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT3(1,2,8); break;                //Win: ALT 128 = Ç
                case EMUL_OS_OSX: OPTION(XKEY(X_9)); break;             //OSX: TAP = Ç with OPTION+9 key
                case EMUL_OS_LINUX: LIN_UTF(0x00C7); break;             //Lin: UTF-00C7 = Ç
            } break;

        case AZ_OEM:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT4(0,1,4,0); break;              //Win: ALT 0140 = Œ
                case EMUL_OS_OSX: SHIFT(OPTION(XKEY(X_O))); break;      //OSX: SHIFT+OPTION+O = Œ
                case EMUL_OS_LINUX: LIN_UTF(0x0152); break;             //Lin: UTF-0152 = Œ
            } break;

        case AZ_OCIRM:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT3(2,2,6); break;                //Win: ALT 226 = Ô
                case EMUL_OS_OSX: SHIFT(OPTION(XKEY(X_LBRC))); break;   //OSX: SHIFT+OPTION+[ = Ô
                default: XCIRCSHIFT(X_O); break;                        //Lin: TAP = Ô with dead ^ + O key
            } break;

        case AZ_UCIRM:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT3(2,3,4); break;                //Win: ALT 234 = Û
                case EMUL_OS_OSX: SHIFT(OPTION(XKEY(X_8))); break;      //OSX: SHIFT+OPTION+8 = Û
                default: XCIRCSHIFT(X_U); break;                        //Lin: TAP = Û with dead ^ + U key
            } break;

        case AZ_AGRVM:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT3(1,8,3); break;//ALT 183 = À
                case EMUL_OS_OSX: SEND_STRING(SS_TAP(X_CAPS) SS_TAP(X_0) SS_TAP(X_CAPS)); break;
                case EMUL_OS_LINUX: LIN_UTF(0x00C0); break;//UTF-00C0 = À
            } break;

        case AZ_EACUM:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT3(1,4,4); break;                //Win: ALT 144 = É
                case EMUL_OS_OSX: XACUTSHIFT(X_E); break;               //OSX: TAP = É with dead ´ + E key
                case EMUL_OS_LINUX: LIN_UTF(0x00C9); break;             //Lin: UTF-00C9 = É
            } break;

        case AZ_EGRVM:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT3(2,1,2); break;                //Win: ALT 212 = È
                case EMUL_OS_OSX: XGRVSHIFT(X_E); break;                //OSX: TAP = È with dead ` + E key
                case EMUL_OS_LINUX: LIN_UTF(0x00C8); break;             //Lin: UTF-00C8 = È
            } break;

        case AZ_ECIRM:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT3(2,1,0); break;                //Win: ALT 210 = Ê
                case EMUL_OS_OSX: SHIFT(OPTION(XKEY(X_E))); break;      //OSX: SHIFT+OPTION+E = Ê
                case EMUL_OS_LINUX: XCIRCSHIFT(X_E); break;             //Lin: TAP = Ê with dead ^ + E key
            } break;

        case AZ_ICIRM:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT3(2,1,5); break;                //Win: ALT 215 = Î
                case EMUL_OS_OSX: SHIFT(OPTION(XKEY(X_H))); break;      //OSX: SHIFT+OPTION+H = Î
                case EMUL_OS_LINUX: XCIRCSHIFT(X_I); break;             //Lin: UTF-00CE = Î
            } break;

        case AZ_ITREM:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT3(2,1,6); break;                //Win: ALT 216 = Ï
                case EMUL_OS_OSX: OPTION(XKEY(X_J)); break;             //OSX: OPTION+J = Ï
                case EMUL_OS_LINUX: LIN_UTF(0x00CF); break;             //Lin: UTF-00CF = Ï
            } break;

        case AZ_UGRVM:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT3(2,3,5); break;                //Win: ALT 235 = Ù
                case EMUL_OS_OSX: OPTION(XKEY(X_QUOT)); break;          //OSX: OPTION+' = Ù
                case EMUL_OS_LINUX: LIN_UTF(0x00D9); break;             //Lin: UTF-00D9 = Ù
            } break;

        case AZ_AEM:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT3(1,4,6); break;                //Win: ALT 146 = Æ
                case EMUL_OS_OSX: SHIFT(OPTION(XKEY(X_Q))); break;      //OSX: SHIFT+OPTION+Q = Æ
                case EMUL_OS_LINUX: LIN_UTF(0x00C6); break;             //Lin: UTF-00C6 = Æ
            } break;

        case AZ_SUP3:
            switch(_emul_os_mode) {
                case EMUL_OS_OSX: /* ??? */ break;            //OSX: Not implemeted
                case EMUL_OS_LINUX: LIN_UTF(0x00B3); break;   //Lin: UTF-00B3 = ³
                case EMUL_OS_WIN: WINALT3(2,5,2); break;       //Win: ALT 252 = ³
            } break;

        case AZ_NTILDM:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT3(1,6,5); break;                //Win: ALT 165 = Ñ
                case EMUL_OS_OSX: XTILDSHIFT(X_N); break;               //OSX: TAP = Ñ with dead ~ + N key
                case EMUL_OS_LINUX: LIN_UTF(0x00D1); break;             //Lin: UTF-00D1 = Ñ
            } break;

        case AZ_RGSTRD:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT4(0,1,7,4); break;                //Win: ALT 174 = ®
                case EMUL_OS_OSX: OPTION(XKEY(X_R)); break;               //OSX: OPTION+R = ®
                case EMUL_OS_LINUX: LIN_UTF(0x00AE); break;               //Lin: UTF-00AE = ®
            } break;

        case AZ_BULLET:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT1(7); break;                //Win: ALT 7 = •
                case EMUL_OS_OSX: OPTION(XKEY(X_NUBS)); break;      //OSX: OPTION+8 = •
                case EMUL_OS_LINUX: LIN_UTF(0x2022); break;         //Lin: UTF-2022 = •
            } break;

        //emul CODE LAYER
        case AZ_EURO:
            if (_emul_os_mode == EMUL_OS_OSX) OPTION(XKEY(X_RBRC))
            else ALTGR(XKEY(X_E));
            break;

        case AZ_LCBR:
            if (_emul_os_mode == EMUL_OS_OSX)  OPTION(XKEY(X_5))
            else ALTGR(XKEY(X_4));
            break; // {

        case AZ_RCBR:  if (_emul_os_mode == EMUL_OS_OSX)  OPTION(XKEY(X_MINS))      else ALTGR(XKEY(X_EQL)); break; // {
        case AZ_LBRC:  if (_emul_os_mode == EMUL_OS_OSX)  SHIFT(OPTION(XKEY(X_5)))  else ALTGR(XKEY(X_5)); break; // [
        case AZ_RBRC:  if (_emul_os_mode == EMUL_OS_OSX)  SHIFT(OPTION(XKEY(X_MINS))) else ALTGR(XKEY(X_MINS)); break; // ]
        case AZ_PLUS:  if (_emul_os_mode == EMUL_OS_OSX)  SHIFT(XKEY(X_SLSH))       else SHIFT(XKEY(X_EQL)); break; // +
        case AZ_EQL:   if (_emul_os_mode == EMUL_OS_OSX)  XKEY(X_SLSH)              else XKEY(X_EQL); break; // =
        case AZ_ASTR:  if (_emul_os_mode == EMUL_OS_OSX)  SHIFT(XKEY(X_RBRC))       else XKEY(X_NUHS); break; // *
        case AZ_PIPE:  if (_emul_os_mode == EMUL_OS_OSX)  SHIFT(OPTION(XKEY(X_L)))  else ALTGR(XKEY(X_6)); break; // |
        case AZ_LABK:  if (_emul_os_mode == EMUL_OS_OSX)  XKEY(X_GRV)               else XKEY(X_NUBS); break; // <
        case AZ_RABK:  if (_emul_os_mode == EMUL_OS_OSX)  SHIFT(XKEY(X_GRV))        else SHIFT(XKEY(X_NUBS)); break; // >
        case AZ_HASH:  if (_emul_os_mode == EMUL_OS_OSX)  SHIFT(XKEY(X_NUBS))       else ALTGR(XKEY(X_3)); break; // #
        case AZ_SML1:  if (_emul_os_mode == EMUL_OS_OSX)  SEND_STRING(SS_TAP(X_DOT)  SS_TAP(X_EQL) SS_TAP(X_MINS)); else SEND_STRING(SS_TAP(X_DOT)  SS_TAP(X_6) SS_TAP(X_MINS)); break;
        case AZ_SML2:  if (_emul_os_mode == EMUL_OS_OSX)  SEND_STRING(SS_TAP(X_COMM) SS_TAP(X_EQL) SS_TAP(X_MINS)); else SEND_STRING(SS_TAP(X_COMM) SS_TAP(X_6) SS_TAP(X_MINS)); break;
        case AZ_SML3:  if (_emul_os_mode == EMUL_OS_OSX)  SEND_STRING(SS_TAP(X_DOT)  SS_TAP(X_EQL) SS_TAP(X_5));    else SEND_STRING(SS_TAP(X_DOT)  SS_TAP(X_6) SS_TAP(X_5)); break;
        // emul NUM
        case AZ_BSLH: if (_emul_os_mode == EMUL_OS_OSX)      SHIFT(OPTION(XKEY(X_DOT)))    else ALTGR(XKEY(X_8)); break;
        case AZ_AT:   if (_emul_os_mode == EMUL_OS_OSX)      XKEY(X_NUBS)                 else ALTGR(XKEY(X_0));        break;

        case AZ_GRV:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT2(9,6); break;                //Win: ALT 96 = ``
                case EMUL_OS_LINUX: LIN_UTF(0x0060); break;           //Lin: UTF-0060 = `
                case EMUL_OS_OSX: XGRV(X_SPC); break;                 //OSX: TAP = ` on space key
            } break;

        case AZ_CIR:
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT2(9,4); break;  // ALT 94 = ^
                default: XCIRC(X_SPC); break;           // OSX, Lin: TAP = ^ on space key
            } break;
                
        case AZ_TIL:   if (_emul_os_mode == EMUL_OS_OSX)  XTILD(X_SPC)              else WINALT3(1,2,6); break; // ALT 126 = ~
            switch(_emul_os_mode) {
                case EMUL_OS_WIN: WINALT3(1,2,6); break;        // Win: ALT 126 = ~
                case EMUL_OS_LINUX: LIN_UTF(0x007E); break;     // Lin: UTF-007E = ~
                case EMUL_OS_OSX: XTILD(X_SPC); break;          // OSX: TAP = ~ on space key
            } break;

        /* case MA_EOL: WINALT2(2,0); break; // ALT 20 = ¶  */
    } // end of main switch
}

bool emul_process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode > AZ_FIRST_EMUL && keycode < AZ_LAST_EMUL  && record->event.pressed) {
        if (keycode > AZ_COMMANDS) {
            switch(keycode) {
                case AZ_REPEAT_TOGGLE: emul_toggle_repeat(); break;
                case AZ_REPEAT_DEC: _emul_repeat_delay = (_emul_repeat_delay > _EMUL_REPEAT_DELAY_MIN) ? _emul_repeat_delay - 10 : _EMUL_REPEAT_DELAY_MIN; break;
                case AZ_REPEAT_INC: _emul_repeat_delay = (_emul_repeat_delay < _EMUL_REPEAT_DELAY_MAX) ? _emul_repeat_delay + 10 : _EMUL_REPEAT_DELAY_MAX; break;
                case AZ_NEXT_EMUL: emul_set_next_emul(); break;
                case AZ_DETECT_OS: emul_detect_os(); break;
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

void emul_matrix_scan_user(void) {
    if (_emul_repeat_active && timer_elapsed(_emul_repeat_timer) > _emul_delay) {
        _emul_send_key(_emul_last_keycode);
        _emul_repeat_timer = timer_read();
        _emul_delay = _emul_repeat_delay;
    }
}

