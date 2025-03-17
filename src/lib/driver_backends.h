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
int driver_backends_init_backend(char *backend_name);

/**
 * @brief Checks if a backend exists and is supported
 * @param backend_name the backend name to check
 * @return 1 is supported, 0 not supported or invalid name
 */
int driver_backends_is_supported(char *backend_name);

/**
 * @brief Print supported backends
 * @description Prints a list of supported backends
 *
 * @return -1 if an error occurred, 0 on success
 */
int driver_backends_print_supported(void);

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




