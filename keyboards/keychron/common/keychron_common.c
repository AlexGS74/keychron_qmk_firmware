/* Copyright 2023 @ Keychron (https://www.keychron.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "keychron_common.h"
#include "raw_hid.h"
#include "version.h"

#ifdef FACTORY_TEST_ENABLE
#    include "factory_test.h"
#    include "keychron_common.h"
#endif

#ifdef LK_WIRELESS_ENABLE
#    include "lkbt51.h"
#endif
#ifdef ANANLOG_MATRIX
#    include "profile.h"
#endif

bool     is_siri_active = false;
uint32_t siri_timer     = 0;

static uint8_t mac_keycode[4] = {
    KC_LOPT,
    KC_ROPT,
    KC_LCMD,
    KC_RCMD,
};

static key_combination_t key_comb_list[4] = {
    {2, {KC_LWIN, KC_TAB}},
    {2, {KC_LWIN, KC_E}},
    {3, {KC_LSFT, KC_LCMD, KC_4}},
    {2, {KC_LWIN, KC_C}},
};

// A struct that stores col and row
typedef struct {
    uint8_t col;
    uint8_t row;
    uint16_t keycode;
} key_pos_t;

key_pos_t mouse_keys_pos[8] = {
    {255, 255, KC_MS_UP},
    {255, 255, KC_MS_DOWN},
    {255, 255, KC_MS_LEFT},
    {255, 255, KC_MS_RIGHT},
    {255, 255, KC_MS_WH_UP},
    {255, 255, KC_MS_WH_DOWN},
    {255, 255, KC_MS_WH_LEFT},
    {255, 255, KC_MS_WH_RIGHT},
};

void set_mouse_key_last_key_pos(uint8_t pos, uint8_t col, uint8_t row) {
    if (pos>7) return;
    mouse_keys_pos[pos].col = col;
    mouse_keys_pos[pos].row = row;
}

uint16_t is_mouse_key(uint8_t col, uint8_t row) {
   for (uint8_t i = 0; i < 8; i++) {
        if (mouse_keys_pos[i].col == col && mouse_keys_pos[i].row == row) {
            return mouse_keys_pos[i].keycode;
        }
    }
    return 0;
}

bool process_record_keychron_common(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_MCTRL:
            if (record->event.pressed) {
                register_code(KC_MISSION_CONTROL);
            } else {
                unregister_code(KC_MISSION_CONTROL);
            }
            return false; // Skip all further processing of this key
        case KC_LNPAD:
            if (record->event.pressed) {
                register_code(KC_LAUNCHPAD);
            } else {
                unregister_code(KC_LAUNCHPAD);
            }
            return false; // Skip all further processing of this key
        case KC_LOPTN:
        case KC_ROPTN:
        case KC_LCMMD:
        case KC_RCMMD:
            if (record->event.pressed) {
                register_code(mac_keycode[keycode - KC_LOPTN]);
            } else {
                unregister_code(mac_keycode[keycode - KC_LOPTN]);
            }
            return false; // Skip all further processing of this key
        case KC_SIRI:
            if (record->event.pressed) {
                if (!is_siri_active) {
                    is_siri_active = true;
                    register_code(KC_LCMD);
                    register_code(KC_SPACE);
                }
                siri_timer = timer_read32();
            } else {
                // Do something else when release
            }
            return false; // Skip all further processing of this key
        case KC_TASK:
        case KC_FILE:
        case KC_SNAP:
        case KC_CTANA:
            if (record->event.pressed) {
                for (uint8_t i = 0; i < key_comb_list[keycode - KC_TASK].len; i++) {
                    register_code(key_comb_list[keycode - KC_TASK].keycode[i]);
                }
            } else {
                for (uint8_t i = 0; i < key_comb_list[keycode - KC_TASK].len; i++) {
                    unregister_code(key_comb_list[keycode - KC_TASK].keycode[i]);
                }
            }
            return false; // Skip all further processing of this key
        case KC_MS_UP:
            set_mouse_key_last_key_pos(0, record->event.key.col, record->event.key.row);
            break;
        case KC_MS_DOWN:
            set_mouse_key_last_key_pos(1, record->event.key.col, record->event.key.row);
            break;
        case KC_MS_LEFT:
            set_mouse_key_last_key_pos(2, record->event.key.col, record->event.key.row);
            break;
        case KC_MS_RIGHT:
            set_mouse_key_last_key_pos(3, record->event.key.col, record->event.key.row);
            break;
        case KC_MS_WH_UP:
            set_mouse_key_last_key_pos(4, record->event.key.col, record->event.key.row);
            break;
        case KC_MS_WH_DOWN:
            set_mouse_key_last_key_pos(5, record->event.key.col, record->event.key.row);
            break;
        case KC_MS_WH_LEFT:
            set_mouse_key_last_key_pos(6, record->event.key.col, record->event.key.row);
            break;
        case KC_MS_WH_RIGHT:
            set_mouse_key_last_key_pos(7, record->event.key.col, record->event.key.row);
            break;
        default:
#ifdef ANANLOG_MATRIX
            return process_record_profile( keycode, record);
#endif
            break;
    }

    return true;
}

void keychron_common_task(void) {
    if (is_siri_active && timer_elapsed32(siri_timer) > 500) {
        unregister_code(KC_LCMD);
        unregister_code(KC_SPACE);
        is_siri_active = false;
        siri_timer     = 0;
    }
#ifdef ANANLOG_MATRIX
    process_profile_select_combo();
#endif
}

#ifdef ENCODER_ENABLE
static void encoder_pad_cb(void *param) {
    encoder_inerrupt_read((uint32_t)param);
}

void encoder_cb_init(void) {
    pin_t encoders_pad_a[] = ENCODERS_PAD_A;
    pin_t encoders_pad_b[] = ENCODERS_PAD_B;
    for (uint32_t i=0; i<NUM_ENCODERS; i++) {
        palEnableLineEvent(encoders_pad_a[i], PAL_EVENT_MODE_BOTH_EDGES);
        palEnableLineEvent(encoders_pad_b[i], PAL_EVENT_MODE_BOTH_EDGES);
        palSetLineCallback(encoders_pad_a[i], encoder_pad_cb, (void*)i);
        palSetLineCallback(encoders_pad_b[i], encoder_pad_cb, (void*)i);
    }
}
#endif
