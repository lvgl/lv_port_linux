/**
 *
 * @file drm.c
 *
 * The DRM/KMS backend
 *
 * Based on the original file from the repository
 *
 * - Move to a separate file
 *   2025 EDGEMTech Ltd.
 *
 * Author: EDGEMTech Ltd, Erik Tagirov (erik.tagirov@edgemtech.ch)
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lvgl/lvgl.h"
#if LV_USE_LINUX_DRM
#include "../simulator_util.h"
#include "../simulator_settings.h"
#include "../backends.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void run_loop_drm(void);
static lv_display_t *init_drm(void);


/**********************
 *  STATIC VARIABLES
 **********************/
static char *backend_name = "DRM";

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Register the backend
 *
 * @param backend the backend descriptor
 * @description configure the descriptor
 */
int backend_init_drm(backend_t *backend)
{
    LV_ASSERT_NULL(backend);

    backend->handle->display = malloc(sizeof(display_backend_t));
    LV_ASSERT_NULL(backend->handle->display);

    backend->handle->display->init_display = init_drm;
    backend->handle->display->run_loop = run_loop_drm;
    backend->name = backend_name;
    backend->type = BACKEND_DISPLAY;

    return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Initialize the DRM display driver
 *
 * @return the LVGL display
 */
static lv_display_t *init_drm(void)
{
    const char *device = getenv_default("LV_LINUX_DRM_CARD", "/dev/dri/card0");
    lv_display_t * disp = lv_linux_drm_create();

    if (disp == NULL) {
        return NULL;
    }

    lv_linux_drm_set_file(disp, device, -1);

    return disp;
}


/**
 * The run loop of the DRM driver
 */
static void run_loop_drm(void)
{
    uint32_t idle_time;

    /* Handle LVGL tasks */
    while (true) {
        /* Returns the time to the next timer execution */
        idle_time = lv_timer_handler();
        usleep(idle_time * 1000);
    }
}

#endif /*#if LV_USE_LINUX_DRM*/
