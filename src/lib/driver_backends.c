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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

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
static backend_t *backends[sizeof(available_backends) / sizeof(available_backends[0])];

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
    backend_init_t init_backend;
    backend_t *b;

    i = 0;
    if (backends[i] != NULL) {
        /* backends are already registered - leave */
        return;
    }

    while ((init_backend = available_backends[i]) != NULL) {

        b = malloc(sizeof(backend_t));
        LV_ASSERT_NULL(b);

        b->handle = malloc(sizeof(backend_handle_t));

        init_backend = available_backends[i];
        LV_ASSERT_NULL(init_backend);

        init_backend(b);
        backends[i] = b;
        i++;
    }
}

int driver_backends_init_backend(char *backend_name)
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

        /*
         * Set default display backend - which is the first defined
         * item in available_backends array
         */
        LV_ASSERT_NULL(backends[0]);
        b = backends[0];

        if (b->type != BACKEND_DISPLAY) {
            LV_LOG_ERROR("The default backend: %s is not a display driver backend", b->name);
            return -1;
        }

        backend_name = backends[0]->name;
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

int driver_backends_print_supported(void)
{
    int i;
    backend_t *b;

    i = 0;
    if (backends[i] == NULL) {
        LV_LOG_ERROR("Please call driver_backends_register first");
        return -1;
    }

    b = backends[i];

    fprintf(stdout, "Default backend: %s\n", b->name);
    fprintf(stdout, "Supported backends: ");

    while ((b = backends[i++]) != NULL) {
        fprintf(stdout, "%s ", b->name);
    }

    fprintf(stdout, "\n");
    return 0;

}

int driver_backends_is_supported(char *backend_name)
{
    char c;
    backend_t *b;
    char *name = backend_name;
    int i = 0;

    while ((c = *backend_name) != '\0') {
        *backend_name = toupper(c);
        backend_name++;
    }

    while ((b = backends[i++]) != NULL) {
        if (strcmp(b->name, name) == 0) {
            return 1;
        }
    }

    return 0;
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

