/**
 * @file simulator_settings.h
 *
 * global simulator settings
 *
 * The simulator settings is a global variable defined in
 * simulator_settings.c
 *
 * Copyright (c) 2025 EDGEMTech Ltd.
 *
 * Author: EDGEMTech Ltd, Erik Tagirov (erik.tagirov@edgemtech.ch)
 *
 */

#ifndef SIMULATOR_SETTINGS_H
#define SIMULATOR_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint32_t window_width;
    uint32_t window_height;
    lv_display_rotation_t rotation;
    bool maximize;
    bool fullscreen;
} simulator_settings_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*SIMULATOR_SETTINGS_H*/
