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
//#define PS2_MOUSE_USE_2_1_SCALING

//#define WS2812_PIO_USE_PIO1
#define WS2812_DI_PIN GP2
#define WS2812_LED_COUNT 10

#define WS2812_TIMING 1200
#define WS2812_T1H 700 // Width of a 1 bit in ns
#define WS2812_T0H 300 // Width of a 0 bit in ns
#define WS2812_TRST_US 200


