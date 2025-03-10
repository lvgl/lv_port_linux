/**
 * @file backends.h
 *
 * Interface for abstration layer of a device backend
 *
 * Copyright (c) 2025 EDGEMTech Ltd.
 *
 * Author: EDGEMTech Ltd, Erik Tagirov (erik.tagirov@edgemtech.ch)
 *
 */
#ifndef BACKENDS_H
#define BACKENDS_H

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
/* Prototype of the display initialization functions */
typedef lv_display_t *(*display_init_t)(void);

/* Prototype of the run loop */
typedef void (*run_loop_t)(void);

/* Represents a display driver handle */
typedef struct {
    display_init_t init_display; /* The display creation/initialization function */
    run_loop_t run_loop;         /* The run loop of the driver handle */
    lv_display_t *display;       /* The LVGL display that was created */
} display_backend_t;

/* Prototype for the initialization of an indev driver backend */
typedef lv_indev_t *(*indev_init_t)(lv_display_t *display);

/* Represents an indev driver backend */
typedef struct {
    indev_init_t init_indev;
} indev_backend_t;

/* Regroup all different types of driver backend */
typedef union {
    display_backend_t *display;
    indev_backend_t *indev;
} backend_handle_t;

/* Define each type of driver backend */
typedef enum {
    BACKEND_DISPLAY,
    BACKEND_INDEV
} backend_type_t;

/* Driver backend descriptor */
typedef struct {
    backend_handle_t *handle;
    char *name;
    backend_type_t type;
} backend_t;

/* Prototype used to register a backend */
typedef int (*backend_init_t)(backend_t *);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/* Graphics backends */
int backend_init_fbdev(backend_t *backend);
int backend_init_drm(backend_t *backend);
int backend_init_sdl(backend_t *backend);
int backend_init_glfw3(backend_t *backend);
int backend_init_wayland(backend_t *backend);
int backend_init_x11(backend_t *backend);

/* Input device driver backends */
int backend_init_evdev(backend_t *backend);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*BACKEND_H*/


