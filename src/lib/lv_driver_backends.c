/******************************************************************
 *
 * lv_driver_backend.c
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
 *
 ******************************************************************/

#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "lvgl/lvgl.h"

#include "lv_simulator_util.h"
#include "lv_simulator_settings.h"
#include "lv_driver_backends.h"

#include "backends.h"
#include "available_backends.h"

/* Internal functions */
static void register_backends(void);

/* Contains the backend descriptors */
static backend_t *backends[BACKEND_CNT] = {NULL};

/* Set once the user selects a backend - or it is set to the default backend */
static backend_t *sel_display_backend = NULL;

/* Contains global simulator settings common to each backend */
simulator_settings_t settings;

/**
 * @brief Register all available backends
 * @description Registers available backends by calling the init function of
 * each backend contained in available_backend array
 * it then and adds the descriptor to the backends array
 */
static void register_backends(void)
{
    int i;
    backend_init_t init_backend;
    backend_t *b;

    if (backends[0] != NULL) {
        /* All available backends were already registered */
        return;
    }

    for (i = 0; i < BACKEND_CNT; i++) {
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


/**
 * @brief Initialize the specified backend
 * @description in case of a display driver backend
 * - create the lv_display, in case of a indev driver backend
 * create an input device
 *
 * @param backend_name the name of the backend to initialize FBDEV,DRM etc
 * @return 0 on success, -1 on error
 */
int lv_driver_backends_init_backend(const char *backend_name)
{
    backend_t *b;
    int i;
    display_backend_t *dispb;
    indev_backend_t *indevb;

    register_backends();

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

/**
 * @brief Retrieve a list of supported driver backends
 *
 * @return an array of strings containing the
 * names of all the supported backends
 */
char **lv_driver_backends_get_supported(void)
{

    backend_t *backend;
    char *backend_name;
    char **list;
    char **supported_backends;
    int i;

    register_backends();

    list = supported_backends = malloc(sizeof(char *) * BACKEND_CNT + 1);

    while ((backend = backends[i]) != NULL) {
        backend_name = malloc(strlen(backend->name));
        strcpy(backend_name, backend->name);

        *list = backend_name;
        list++;
        i++;
    }

    /* Sentinel - terminate list */
    *list = NULL;

    return supported_backends;
}

/**
 * @brief Enter the run loop
 * @description enter the run loop of the selected backend
 */
void lv_driver_backends_run_loop(void)
{
    display_backend_t *dispb;

    if (sel_display_backend != NULL && sel_display_backend->handle->display != NULL) {

        dispb = sel_display_backend->handle->display;
        dispb->run_loop();

    } else {
        LV_LOG_ERROR("No backend has been selected - initialize the backend first");
    }
}
