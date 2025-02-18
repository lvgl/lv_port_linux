/**
 * @file driver_backends.c
 *
 * Copyright (c) 2025 EDGEMTech Ltd.
 *
 * Author: EDGEMTech Ltd, Erik Tagirov (erik.tagirov@edgemtech.ch)
 */

/*********************
 *      INCLUDES
 *********************/

#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "lvgl/lvgl.h"

#include "simulator_util.h"
#include "simulator_settings.h"
#include "driver_backends.h"

#include "backends.h"

/*********************
 *      DEFINES
 *********************/

/* Catch configuration errors at compile time - checks if no backend was selected */
#if LV_USE_SDL == 0 && \
    LV_USE_WAYLAND == 0 && \
    LV_USE_LINUX_DRM == 0 && \
    LV_USE_OPENGLES == 0 && \
    LV_USE_X11 == 0 && \
    LV_USE_LINUX_FBDEV == 0

#error Unsupported configuration - Please select at least one graphics backend in lv_conf.h
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/* Internal functions */
static void register_backends(void);

/**********************
 *  STATIC VARIABLES
 **********************/

/* The default backend is the one that will end up at index 0
 * To add support for a new driver backend add the declaration
 * and append an entry to the available_backends array
 */
backend_init_t available_backends[] = {

#if LV_USE_LINUX_FBDEV
    backend_init_fbdev,
#endif

#if LV_USE_LINUX_DRM
    backend_init_drm,
#endif

#if LV_USE_SDL
    backend_init_sdl,
#endif

#if LV_USE_WAYLAND
    backend_init_wayland,
#endif

#if LV_USE_X11
    backend_init_x11,
#endif

#if LV_USE_OPENGLES
    backend_init_glfw3,
#endif

#if LV_USE_EVDEV
    backend_init_evdev,
#endif
    NULL    /* Sentinel */
};

/* Contains the backend descriptors */
static backend_t *backends[sizeof(available_backends) / sizeof(backend_init_t)];

/* Set once the user selects a backend - or it is set to the default backend */
static backend_t *sel_display_backend = NULL;

/**********************
 *  GLOBAL VARIABLES
 **********************/
/* Contains global simulator settings common to each backend */
simulator_settings_t settings;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void driver_backends_register(void)
{
    int i;
    int cnt;
    backend_init_t init_backend;
    backend_t *b;

    cnt = sizeof(available_backends) / sizeof(backend_init_t);

    for (i = 0; i < cnt; i++) {
        if (available_backends[i] != NULL) {

            b = malloc(sizeof(backend_t));
            LV_ASSERT_NULL(b);

            b->handle = malloc(sizeof(backend_handle_t));

            init_backend = available_backends[i];
            LV_ASSERT_NULL(init_backend);

            init_backend(b);
            backends[i] = b;
        }
    }
}

int driver_backends_init_backend(const char *backend_name)
{
    backend_t *b;
    int i;
    display_backend_t *dispb;
    indev_backend_t *indevb;

    if (backends[0] == NULL) {
        LV_LOG_ERROR("Please call driver_backends_register first");
        return -1;
    }

    if (backend_name == NULL) {
        /* set default display backend */
        LV_ASSERT_NULL(backends[0]);
        b = backends[0];

        if (b->type != BACKEND_DISPLAY) {
            LV_LOG_ERROR("The default backend: %s is not a display driver backend", b->name);
            return -1;
        }
    }

    i = 0;
    while ((b = backends[i]) != NULL) {

        /* Check if such a backend exists */
        if (strcmp(b->name, backend_name) == 0) {

            if (b->type == BACKEND_DISPLAY) {
                /* Initialize the display */

                dispb = b->handle->display;
                LV_ASSERT_NULL(dispb->init_display);
                dispb->display = dispb->init_display();

                if (dispb->display == NULL) {
                    LV_LOG_ERROR("Failed to init display with %s backend", b->name);
                    return -1;
                }

                sel_display_backend = b;
                LV_LOG_INFO("Initialized %s display backend", b->name);
                break;

            } else if (b->type == BACKEND_INDEV) {
                /* Initialize input device */

                indevb = b->handle->indev;
                LV_ASSERT_NULL(indevb->init_indev);

                /* The display driver backend - has to be initialized first */
                if (sel_display_backend == NULL) {
                    LV_LOG_ERROR(
                        "Failed to init indev backend: %s - display needs to be initialized",
                        b->name);
                    return -1;
                }

                LV_LOG_INFO("Initialized %s indev backend", b->name);

                dispb = sel_display_backend->handle->display;

                LV_ASSERT_NULL(dispb->display);
                indevb->init_indev(dispb->display);
                break;
            }
        }
        i++;
    }

    return 0;
}

char **driver_backends_get_supported(void)
{

    backend_t *backend;
    char *backend_name;
    char **list;
    char **supported_backends;
    int i;
    int cnt;

    i = 0;
    cnt = sizeof(available_backends) / sizeof(backend_init_t);

    if (backends[0] == NULL) {
        LV_LOG_ERROR("Please call driver_backends_register first");
        return NULL;
    }

    list = supported_backends = malloc(sizeof(char *) * cnt + 1);
    LV_ASSERT_NULL(list);

    while ((backend = backends[i]) != NULL) {

        if (backend->type == BACKEND_DISPLAY) {
            backend_name = malloc(strlen(backend->name));
            strcpy(backend_name, backend->name);

            *list = backend_name;
            list++;
        }
        i++;
    }

    /* Sentinel - terminate list */
    *list = NULL;

    return supported_backends;
}


void driver_backends_run_loop(void)
{
    display_backend_t *dispb;

    if (sel_display_backend != NULL && sel_display_backend->handle->display != NULL) {

        dispb = sel_display_backend->handle->display;
        dispb->run_loop();

    } else {
        LV_LOG_ERROR("No backend has been selected - initialize the backend first");
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

