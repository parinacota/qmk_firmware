// Copyright 2024 parinacota (@parinacota)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
//#define NO_DEBUG

/* disable print */
//#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT


#define TAPPING_TOGGLE 1
#define TAPPING_TERM 200

#define LED_PIN_ON_STATE 0

#define PS2_MOUSE_ROTATE 270
#define MOUSE_LAYER_TIMEOUT 1000

#define WS2812_PIO_USE_PIO1
#define WS2812_DI_PIN GP2
#define RGBLIGHT_LED_COUNT 1
#define RGBLIGHT_LIMIT_VAL 96
#define RGBLIGHT_DEFAULT_MODE RGBLIGHT_MODE_STATIC_LIGHT
#define RGBLIGHT_DEFAULT_ON true
#define RGBLIGHT_DEFAULT_HUE 0
#define RGBLIGHT_DEFAULT_VAL RGBLIGHT_LIMIT_VAL
#define RGBLIGHT_LAYERS
#define RGBLIGHT_SLEEP





