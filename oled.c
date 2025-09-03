#include QMK_KEYBOARD_H

#ifdef OLED_ENABLE

#include "wpm.h"
#include <stdio.h>

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (!is_keyboard_master()) {
        return OLED_ROTATION_180; // flips the display 180 degrees if offhand
    }
    return OLED_ROTATION_270;
}


static void oled_render_layer_state(void) {
    oled_set_cursor(0, 5);

    if (get_highest_layer(default_layer_state) == 0) {
        oled_write(PSTR("QWERT"), false);
    } else {
        oled_write(PSTR("COLMD"), false);
    }

    switch (get_highest_layer(layer_state)) {
        case 2:
            oled_write(PSTR("Media"), false);
            break;
        case 3:
            oled_write_ln(PSTR("Navi"), false);
            break;
        case 4:
            oled_write(PSTR("Mouse"), false);
            break;
        case 5:
            oled_write(PSTR("Symbl"), false);
            break;
        case 6:
            oled_write(PSTR("Numbr"), false);
            break;
        case 7:
            oled_write(PSTR("Funct"), false);
            break;
        default:
            oled_write_ln(PSTR("Base"), false);
            break;
    }
}

#define WPM_BAR_MAX   70   // «100%» для шкалы (подстройте под себя)
#define WPM_BAR_COLS  5    // ширина полоски внутри скобок

static void oled_render_wpm(void) {
    uint8_t wpm = get_current_wpm();

    // Метка
    oled_set_cursor(1, 8);
    oled_write_P(PSTR("WPM"), false);

    // Три цифры WPM
    char num[4];
    snprintf(num, sizeof(num), "%03u", wpm);

    // Сколько "клеток" заполняем
    uint8_t capped = (wpm > WPM_BAR_MAX) ? WPM_BAR_MAX : wpm;
    uint8_t filled = (uint8_t)((uint32_t)capped * WPM_BAR_COLS / WPM_BAR_MAX);

    // 1) Полоска без скобок, символ 0xA4 = "полная заливка"
    oled_set_cursor(0, 9);
    for (uint8_t i = 0; i < WPM_BAR_COLS; i++) {
        oled_write_char((i < filled) ? (char)0xA4 : ' ', false);
    }

    // 2) Поверх — цифры; если цифра попадает на заливку -> инвертируем (чёрная на белом)
    const uint8_t num_col0 = 1;   // стартовая колонка числа (можно сдвинуть/центрировать)
    for (uint8_t i = 0; i < 3; i++) {
        bool overlap = (i < filled);                // т.к. число начинается с той же колонки
        oled_set_cursor(num_col0 + i, 9);
        oled_write_char(num[i], overlap);           // overlap==true -> инвертированная цифра
    }
}

static const char * const effect_names[] PROGMEM = {
    [1]  = "SOLID COLOR",
    [2]  = "ALPHAS MODS",
    [3]  = "GRADIENT UP DOWN",
    [4]  = "GRADIENT LEFT RIGHT",
    [5]  = "BREATHING",
    [6]  = "BAND SAT",
    [7]  = "BAND VAL",
    [8]  = "BAND PINWHEEL SAT",
    [9]  = "BAND PINWHEEL VAL",
    [10] = "BAND SPIRAL SAT",
    [11] = "BAND SPIRAL VAL",
    [12] = "CYCLE ALL",
    [13] = "CYCLE LEFT RIGHT",
    [14] = "CYCLE UP DOWN",
    [15] = "RAINBOW MOVING CHEVRON",
    [16] = "CYCLE OUT IN",
    [17] = "CYCLE OUT IN DUAL",
    [18] = "CYCLE PINWHEEL",
    [19] = "CYCLE SPIRAL",
    [20] = "DUAL BEACON",
    [21] = "RAINBOW BEACON",
    [22] = "RAINBOW PINWHEELS",
    [23] = "RAINDROPS",
    [24] = "JELLYBEAN RAINDROPS",
    [25] = "HUE BREATHING",
    [26] = "HUE PENDULUM",
    [27] = "HUE WAVE",
    [28] = "PIXEL RAIN",
    [29] = "PIXEL FLOW",
    [30] = "PIXEL FRACTAL",
    [31] = "TYPING HEATMAP",
    [32] = "DIGITAL RAIN",
    [33] = "REACTIVE SIMPLE",
    [34] = "REACTIVE",
    [35] = "REACTIVE WIDE",
    [36] = "REACTIVE MULTIWIDE",
    [37] = "REACTIVE CROSS",
    [38] = "REACTIVE MULTICROSS",
    [39] = "REACTIVE NEXUS",
    [40] = "REACTIVE MULTINEXUS",
    [41] = "SPLASH",
    [42] = "MULTISPLASH",
    [43] = "SOLID SPLASH",
    [44] = "SOLID MULTISPLASH",
};

static const char* effect_name(uint8_t mode) {
    if (mode < ARRAY_SIZE(effect_names) && effect_names[mode]) {
        return (const char*)pgm_read_ptr(&effect_names[mode]);
    }

    char buf[3];
    return utoa(mode, buf, 10);
}

static void render_rgb_mode_line(void) {
    bool    is_rgb_enabled   = rgb_matrix_is_enabled();

    if (is_rgb_enabled) {
	    uint8_t rgb_mode = rgb_matrix_get_mode();
        char rgb_mode_name[22];
        snprintf(rgb_mode_name, sizeof(rgb_mode_name), "%-21s", effect_name(rgb_mode));
        oled_write(rgb_mode_name, false);
    } else {
        oled_write_ln_P(PSTR("RGB OFF"), false);
    }
}

static inline uint8_t rgb_brightness_max(void) {
    #if defined(RGB_MATRIX_ENABLE)
    	#  ifdef RGB_MATRIX_MAXIMUM_BRIGHTNESS
        	return RGB_MATRIX_MAXIMUM_BRIGHTNESS;
    	#  else
        	return 255;
    	#  endif
    #else
        return 255;
    #endif
}

static void render_rgb_params_line(void) {
	bool is_rgb_enabled = rgb_matrix_is_enabled();

	if (is_rgb_enabled) {
		hsv_t 	hsv = rgb_matrix_get_hsv();
		uint8_t speed = rgb_matrix_get_speed();

		uint8_t satPercent = (uint8_t)(((uint16_t)hsv.s * 100) / 255);
		uint8_t valPercent = (uint8_t)(((uint16_t)hsv.v * 100) / rgb_brightness_max()) ;
		uint8_t spdPercent = (uint8_t)(((uint16_t)speed * 100) / 255) ;

		char hueChar[4];
		snprintf(hueChar, sizeof hueChar, "%03u", hsv.h);

		oled_set_cursor(0, 2);
		oled_write(PSTR("HUE"), false);
		oled_set_cursor(0, 3);
		oled_write(hueChar, false);

		char satChar[4];
		snprintf(satChar, sizeof satChar, "%03u", satPercent);

		oled_set_cursor(5, 2);
		oled_write(PSTR("SAT"), false);
		oled_set_cursor(5, 3);
		oled_write(satChar, false);

		char valChar[4];
		snprintf(valChar, sizeof valChar, "%03u", valPercent);

		oled_set_cursor(10, 2);
		oled_write(PSTR("BRI"), false);
		oled_set_cursor(10, 3);
		oled_write(valChar, false);

		char spdChar[4];
		snprintf(spdChar, sizeof spdChar, "%03u", spdPercent);

		oled_set_cursor(15, 2);
		oled_write(PSTR("SPD"), false);
		oled_set_cursor(15, 3);
		oled_write(spdChar, false);
    }
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        oled_render_layer_state();
        oled_render_wpm();
    } else {
        render_rgb_mode_line();
		render_rgb_params_line();
    }
    return false;
}

#endif // OLED_ENABLE