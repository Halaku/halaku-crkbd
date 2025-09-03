VIA_ENABLE          = yes
VIAL_ENABLE         = yes
LTO_ENABLE          = yes
VIALRGB_ENABLE      = no

RGBLIGHT_ENABLE     = no
RGB_MATRIX_ENABLE   = yes # Can't have RGBLIGHT and RGB_MATRIX at the same time.
RGB_MATRIX_DRIVER   = ws2812
WS2812_DRIVER 		= vendor

WPM_ENABLE          = yes
MOUSEKEY_ENABLE     = yes
OLED_ENABLE         = yes
OLED_DRIVER         = ssd1306
EXTRAKEY_ENABLE     = yes
COMBO_ENABLE        = no
NKRO_ENABLE         = yes

QMK_SETTINGS        = yes
EEPROM_DRIVER       = wear_leveling
WEAR_LEVELING_DRIVER = rp2040_flash

SRC += oled.c