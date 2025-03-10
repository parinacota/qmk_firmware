#ifndef EMUL_AZERTY_H
#define EMUL_AZERTY_H

// Emulate special characters for AZERTY layout
// Usage:
// 0. add these options in rules.mk:
//      OS_DETECTION_ENABLE = yes
//      DEFERRED_EXEC_ENABLE = yes
//      UNICODE_ENABLE = yes
//      SRC += lib/emul_azerty.c
// 1. Add <emul_azerty.h> to your keymap.c
// 2. Add the following code to your config.h
//      call emul_keyboard_post_init_user() in your keyboard_post_init_user() --> will detect the host OS -- 
//      call process_record_user_emul(keycode, record) in your process_record_user() --> will emulate the keycodes
//      call matrix_scan_user_emul() in your matrix_scan_user() --> needed for the repeatition of the keycodes (if used)
// 3. define your custom keycodes in your keymap.c like this:
//      enum custom_keycodes_emul {
//         MY_FIRST_CUSTOM_KEYCODE = NEW_SAFE_RANGE, // NEW_SAFE_RANGE is the last keycode of the emul layer, that starts its counter with SAFE_RANGE
//         ...
//      }
// 4. Use the AZ_xxxx keycodes in your keymaps
// 5. optionnaly:
//      - call emul_set_os() to force a specific OS (EMUL_OS_WIN, EMUL_OS_OSX, EMUL_OS_LINUX)
//      - call emul_get_os() to get the current OS (EMUL_OS_WIN or EMUL_OS_OSX)
//      - use the keycodes AZ_REPEAT_TOGGLE, AZ_REPEAT_DEC, AZ_REPEAT_INC to control key repeat
//      - use the keycodes AZ_NEXT_EMUL to switch between OS
//      - implement the emul_notify_event_callback(emul_event_t ev) to get the OS change events or other events (like repeat on/off)
// 6. Enjoy

#include QMK_KEYBOARD_H 
#include <keymap_french.h>

#define _EMUL_INITIAL_REPEAT_DELAY 300 
#define _EMUL_REPEAT_DELAY 70
#define _EMUL_REPEAT_DELAY_MIN 30
#define _EMUL_REPEAT_DELAY_MAX 200

//Normal keycodes for AZERTY layout
#define AZ_Q FR_Q
#define AZ_W FR_W
#define AZ_E FR_E
#define AZ_R FR_R
#define AZ_T FR_T
#define AZ_Y FR_Y
#define AZ_U FR_U
#define AZ_I FR_I
#define AZ_O FR_O
#define AZ_P FR_P
#define AZ_A FR_A
#define AZ_S FR_S
#define AZ_D FR_D
#define AZ_F FR_F
#define AZ_G FR_G
#define AZ_H FR_H
#define AZ_J FR_J
#define AZ_K FR_K
#define AZ_L FR_L
#define AZ_Z FR_Z
#define AZ_X FR_X
#define AZ_C FR_C
#define AZ_V FR_V
#define AZ_B FR_B
#define AZ_N FR_N
#define AZ_M FR_M
#define AZ_DOT FR_DOT
#define AZ_COMM FR_COMM
#define AZ_CCED FR_CCED
#define AZ_DEG FR_DEG
#define AZ_DIAE FR_DIAE
#define AZ_EACU FR_EACU
#define AZ_EGRV FR_EGRV
#define AZ_AGRV FR_AGRV
#define AZ_UGRV FR_UGRV
#define AZ_DQUO FR_DQUO
#define AZ_AMPR FR_AMPR
#define AZ_PERC FR_PERC
#define AZ_DLR FR_DLR
#define AZ_LPRN FR_LPRN
#define AZ_RPRN FR_RPRN
#define AZ_QUOT FR_QUOT
#define AZ_1 FR_1
#define AZ_2 FR_2
#define AZ_3 FR_3
#define AZ_4 FR_4
#define AZ_5 FR_5
#define AZ_6 FR_6
#define AZ_7 FR_7
#define AZ_8 FR_8
#define AZ_9 FR_9
#define AZ_0 FR_0
#define AZ_SLSH FR_SLSH                    

//emulated keycodes for AZERTY layout
enum custom_keycodes_emul { 
    AZ_FIRST_EMUL = SAFE_RANGE, 
    AZ_EURO, 
    AZ_AT,  
    AZ_MINS, 

    /*emul SHIFT LAYER*/ 
    AZ_EXCL, 
    AZ_INSEC, 
    AZ_SCLN, 
    AZ_COLN, 
    AZ_DEL, 
    AZ_BSLH, 

    /*emul DEAD LAYER*/ 
    AZ_ACIR, 
    AZ_OE, 
    AZ_OCIR, 
    AZ_UCIR, 
    AZ_ECIR, 
    AZ_ICIR, 
    AZ_ITRE, 
    AZ_AE, 
    AZ_ETC, 
    AZ_QUOTFR, 
    AZ_GRV, 
    AZ_CADR, 
    AZ_SCADR, 
    AZ_QCADR, 
    AZ_LQUOTFR, 
    AZ_RQUOTFR, 
    AZ_UNDS, 
    AZ_NTILD, 
    AZ_MICR, 
    AZ_SUP2, 
    AZ_SECT, 
    AZ_COPYR, 
    AZ_BULLET, 
    AZ_https, 

    /*emul DEAD MAJ LAYER*/ 
    AZ_ACIRM, 
    AZ_CCEDM, 
    AZ_OEM, 
    AZ_OCIRM, 
    AZ_UCIRM, 
    AZ_AGRVM, 
    AZ_EACUM, 
    AZ_EGRVM, 
    AZ_ECIRM, 
    AZ_ICIRM, 
    AZ_ITREM, 
    AZ_UGRVM, 
    AZ_AEM, 
    AZ_SUP3, 
    AZ_NTILDM, 
    AZ_RGSTRD, 

    /*emul CODE LAYER*/ 
    AZ_CIR, 
    AZ_TIL, 
    AZ_LCBR, 
    AZ_RCBR, 
    AZ_LBRC, 
    AZ_RBRC, 
    AZ_PLUS, 
    AZ_ASTR, 
    AZ_EQL, 
    AZ_PIPE, 
    AZ_LABK, 
    AZ_RABK, 
    AZ_HASH, 
    AZ_SML1, 
    AZ_SML2, 
    AZ_SML3,
    
    AZ_COMMANDS,
    AZ_REPEAT_TOGGLE,
    AZ_REPEAT_DEC,
    AZ_REPEAT_INC,
    AZ_NEXT_EMUL,

    AZ_LAST_EMUL,
    NEW_SAFE_RANGE
};

/// @brief emulated OS types
typedef enum {
    EMUL_OS_WIN,
    EMUL_OS_LINUX,
    EMUL_OS_OSX,
} emul_os_types;

/// @brief events of the emulation layer (for the callback)
typedef enum {
    EMUL_EVENT_OS_WIN,
    EMUL_EVENT_OS_OSX,
    EMUL_EVENT_OS_LINUX,
    EMUL_EVENT_REPEAT_ON,
    EMUL_EVENT_REPEAT_OFF
} emul_event_t;

/// @brief internal structure for the tap dance pair of keycodes
typedef struct {
    uint16_t kc1;
    uint16_t kc2;
    uint16_t kc_held;
} _emul_tap_dance_pair_invert_t;

/// @brief Set the OS to emulate (OSX, Linux, Win)
/// @param os The OS to emulate (EMUL_OS_WIN, EMUL_OS_LINUX, EMUL_OS_OSX)
void emul_set_os(emul_os_types os);

/// @brief Switch to the next emualted OS
/// @param none 
void emul_set_next_emul(void);

/// @brief Return the current emulated OS
/// @param none
/// @return The current emulated OS (EMUL_OS_WIN, EMUL_OS_LINUX, EMUL_OS_OSX)
emul_os_types emul_get_os(void);

/// @brief Toggle the key repeatition
/// @param none
/// @return The new state of the key repeatition (true = on, false = off)
bool emul_toggle_repeat(void);

/// @brief Callback to get the host OS through USB discovery (QMK OS detection)
/// @param trigger_time 
/// @param cb_arg (not used)
/// @return 0 (never repeat)
uint32_t _emul_get_host_os(uint32_t trigger_time, void *cb_arg);

/// @brief internal function to invert keycodes according of the OS (as CTRL / COMMAND on OSX)
/// @param state 
/// @param user_data 
void _emul_tap_dance_pair_invert_on_each_tap(tap_dance_state_t *state, void *user_data);

/// @brief internal function to invert keycodes according of the OS (as CTRL / COMMAND on OSX)
/// @param state 
/// @param user_data 
void _emul_tap_dance_pair_invert_finished(tap_dance_state_t *state, void *user_data);

/// @brief internal function to invert keycodes according of the OS (as CTRL / COMMAND on OSX)
/// @param state 
/// @param user_data 
void _emul_tap_dance_pair_invert_reset(tap_dance_state_t *state, void *user_data);

/// @brief Initialize the emul layer (call OS detection)
/// @param  none
void emul_keyboard_post_init_user(void);

/// @brief internal function to send a keycode to the host, emulating the azerty layout
/// @param keycode AMK keycode to send (range AZ_xxx)
void _emul_send_key(uint16_t keycode);

/// @brief Function to be called in the process_record_user() to emulate the azerty layout
/// @param keycode trasmit the keycode to the emul layer
/// @param record transmit the record to the emul layer
/// @return true
bool emul_process_record_user(uint16_t keycode, keyrecord_t *record);

/// @brief Function to be called in the matrix_scan_user() to emulate the azerty layout (to implement the repeatition of the keycodes)
/// @param  none
void emul_matrix_scan_user(void);

/// TAP DANCE for a pair of keycodes, inverting the keycodes according the OS (as CTRL / COMMAND on OSX)
#define EMUL_ACTION_TAP_DANCE_DOUBLE_INVERT_OSX(kc1, kc2) \
    { .fn = {_emul_tap_dance_pair_invert_on_each_tap, _emul_tap_dance_pair_invert_finished, _emul_tap_dance_pair_invert_reset, NULL}, .user_data = (void *)&((_emul_tap_dance_pair_invert_t){kc1, kc2}), }

#endif