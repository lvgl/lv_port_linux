/**
 * @file x11.c
 *
 * The backend for the X11 windowing system
 *
 * Based on the original file from the repository.
 *
 * Move to a separate file
 * 2025 EDGEMTech Ltd.
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
#if LV_USE_X11
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
 *  EXTERNAL VARIABLES
 **********************/
extern simulator_settings_t settings;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_display_t *init_x11(void);
static void run_loop_x11(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static char *backend_name = "X11";

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
 * @description configures the descriptor
 */
int backend_init_x11(backend_t *backend)
{
    LV_ASSERT_NULL(backend);
    backend->handle->display = malloc(sizeof(display_backend_t));
    LV_ASSERT_NULL(backend->handle->display);

    backend->name = backend_name;
    backend->handle->display->init_display = init_x11;
    backend->handle->display->run_loop = run_loop_x11;
    backend->type = BACKEND_DISPLAY;

    return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Initialize the X11 display driver
 *
 * @return the LVGL display
 */
static lv_display_t *init_x11(void)
{
    lv_display_t *disp;
    LV_IMG_DECLARE(mouse_cursor_icon);

    disp = lv_x11_window_create("LVGL simulator",
            settings.window_height, settings.window_width);

    disp = lv_display_get_default();

    if (disp == NULL) {
        return NULL;
    }

    lv_x11_inputs_create(disp, &mouse_cursor_icon);

    return disp;
}

/**
 * The run loop of the X11 driver
 */
void run_loop_x11(void)
{
    uint32_t idle_time;

    /* Handle LVGL tasks */
    while (true) {
        /* Returns the time to the next timer execution */
        idle_time = lv_timer_handler();
        usleep(idle_time * 1000);
    }
}

#endif /*#if LV_USE_X11*/
