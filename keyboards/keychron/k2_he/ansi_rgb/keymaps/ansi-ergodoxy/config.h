//
// Created by Alex Hubitski on 12/8/24.
//

#ifndef QMK_FIRMWARE_CONFIG_H
#define QMK_FIRMWARE_CONFIG_H

#endif // QMK_FIRMWARE_CONFIG_H

#define AUTO_SHIFT_TIMEOUT 175
#define NO_AUTO_SHIFT_TAB

#undef  DEBOUNCE
#define DEBOUNCE 36
#define REPEAT_DELAY 300
#define TAP_HOLD 150

#define ONESHOT_TAP_TOGGLE 3

#define TAPPING_TERM 180
#define TAPPING_FORCE_HOLD
#define QUICK_TAP_TERM 0
#define HOLD_ON_OTHER_KEY_PRESS //PERMISSIVE_HOLD

//#undef IGNORE_MOD_TAP_INTERRUPT

//#define MOUSEKEY_INTERVAL 27
//#define MOUSEKEY_MAX_SPEED 1
//#define MOUSEKEY_TIME_TO_MAX 60
#define MOUSEKEY_VARIABLE 1

//#undef  TRAVEL_SCALE
//#define TRAVEL_SCALE 12
