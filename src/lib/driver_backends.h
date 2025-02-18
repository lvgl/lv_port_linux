/**
 * @file driver_backends.h
 *
 * provides an abstration to support multiple graphical
 * driver backends at the same time whitout recompiling everything
 * each time
 *
 * E.g: this means LVGL can be compiled with both SDL or X11
 *
 * - see backend.h for the details on the interface.
 * - see the files in display_backends directory for examples
 *   on how to use each driver
 *
 * Copyright (c) 2025 EDGEMTech Ltd.
 *
 * Author: EDGEMTech Ltd, Erik Tagirov (erik.tagirov@edgemtech.ch)
 */

#ifndef DRIVER_BACKENDS_H
#define DRIVER_BACKENDS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*
 * Register all available backends
 * This function must be called first before any other
 * function
 */
void driver_backends_register(void);

/**
 * Initialize the specified backend
 * @description in case of a display driver backend
 * - create the lv_display, in case of a indev driver backend
 * create an input device
 *
 * @param backend_name the name of the backend to initialize FBDEV,DRM etc
 * @return 0 on success, -1 on error
 */
int driver_backends_init_backend(const char *backend_name);

/**
 * Retrieve a list of supported driver backends
 *
 * @return an array of strings containing the
 * names of all the supported backends
 */
char **driver_backends_get_supported(void);

/**
 * @brief Enter the run loop
 * @description enter the run loop of the selected backend
 */
void driver_backends_run_loop(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*DRIVER_BACKENDS_H*/




