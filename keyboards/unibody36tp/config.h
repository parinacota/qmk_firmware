// Copyright 2024 parinacota (@parinacota)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET // Activates the double-tap behavior
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 400U // Timeout window in ms in which the double tap can occur.

#define TAPPING_TOGGLE 1
#define TAPPING_TERM 200

#define NO_USB_STARTUP_CHECK
//#define OS_DETECTION_DEBOUNCE 500
#define OS_DETECTION_KEYBOARD_RESET
//#define OS_DETECTION_SINGLE_REPORT

// HAPTIC
#define I2C_DRIVER I2CD0
#define I2C1_SDA_PIN GP16
#define I2C1_SCL_PIN GP17
#define DRV2605L_FB_ERM_LRA 1
#define DRV2605L_FB_BRAKEFACTOR 3 /* For 1x:0, 2x:1, 3x:2, 4x:3, 6x:4, 8x:5, 16x:6, Disable Braking:7 */
#define DRV2605L_FB_LOOPGAIN 1 /* For  Low:0, Medium:1, High:2, Very High:3 */
/* Please refer to your datasheet for the optimal setting for your specific motor. */
#define DRV2605L_RATED_VOLTAGE 1.2
#define DRV2605L_V_PEAK 1.3
#define DRV2605L_V_RMS 1.2
#define DRV2605L_F_LRA 170 /* resonance freq */
#define DRV2605L_LIBRARY 6
#define DRV2605L_GREETING 12
#define DRV2605L_DEFAULT_MODE 26
#define NO_HAPTIC_ALPHA
#define NO_HAPTIC_PUNCTUATION
#define NO_HAPTIC_NUMERIC

// PS2
#define PS2RESETPIN GP14
#define PS2_MOUSE_ROTATE 270
#define MOUSE_LAYER_TIMEOUT 1000
#define PS2_MOUSE_USE_2_1_SCALING





