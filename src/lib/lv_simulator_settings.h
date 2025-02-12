/*******************************************************************
 *
 * lv_simulator_settings.h global simulator settings
 *
 * The simulator settings is a global variable defined in
 * lv_driver_backends.c
 *
 * Copyright (c) 2025 EDGEMTech Ltd.
 *
 * Author: EDGEMTech Ltd, Erik Tagirov (erik.tagirov@edgemtech.ch)
 *
 ******************************************************************/
typedef struct {
    uint32_t window_width;
    uint32_t window_height;
    bool maximize;
    bool fullscreen;
} simulator_settings_t;
