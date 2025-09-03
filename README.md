# Keymap for crkbd with Vial support

Custom keymap `halaku-crkbd` for the Corne keyboard (crkbd), compatible with Vial firmware.

## Building the firmware

To build the firmware for RP2040 (elite_pi):

`qmk compile -c -kb crkbd/rev1 -km halaku-crkbd -e CONVERT_TO=elite_pi`


## Working with keymap in JSON format

Convert your keymap to JSON format for editing in graphical configurators like Vial or QMK Configurator:

`qmk c2json -kb crkbd/rev1 -km halaku-crkbd -o keymap.json –no-cpp`


Edit the `keymap.json` file online at:  
https://config.qmk.fm/#/crkbd/rev1/LAYOUT_split_3x6_3

Convert the edited JSON keymap back to a C keymap file with:

`qmk json2c ~/Downloads/keymap.json -o ~/vial-qmk/keyboards/crkbd/keymaps/halaku-crkbd/keymap.c`