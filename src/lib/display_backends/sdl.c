/**
 * @file sdl.c
 *
 * The backend for the SDL simulator
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
#if LV_USE_SDL
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
static void run_loop_sdl(void);
static lv_display_t * init_sdl(void);

/**********************
 *  STATIC VARIABLES
 **********************/

static char * backend_name = "SDL";

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Register the backend
 * @param backend the backend descriptor
 * @description configures the descriptor
 */
int backend_init_sdl(backend_t * backend)
{
    LV_ASSERT_NULL(backend);

    backend->handle->display = malloc(sizeof(display_backend_t));
    LV_ASSERT_NULL(backend->handle->display);

    backend->handle->display->init_display = init_sdl;
    backend->handle->display->run_loop = run_loop_sdl;
    backend->name = backend_name;
    backend->type = BACKEND_DISPLAY;

    return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Initialize the SDL display driver
 *
 * @return the LVGL display
 */
static lv_display_t * init_sdl(void)
{

    lv_display_t * disp = lv_sdl_window_create(settings.window_width, settings.window_height);

    if(disp == NULL) {
        return NULL;
    }

    lv_indev_t * mouse = lv_sdl_mouse_create();
    lv_indev_set_group(mouse, lv_group_get_default());
    lv_indev_set_display(mouse, disp);

    LV_IMAGE_DECLARE(mouse_cursor_icon);
    lv_obj_t * cursor_obj;
    cursor_obj = lv_image_create(lv_screen_active());
    lv_image_set_src(cursor_obj, &mouse_cursor_icon);
    lv_indev_set_cursor(mouse, cursor_obj);

    lv_indev_t * mousewheel = lv_sdl_mousewheel_create();
    lv_indev_set_display(mousewheel, disp);
    lv_indev_set_group(mousewheel, lv_group_get_default());

    lv_indev_t * kb = lv_sdl_keyboard_create();
    lv_indev_set_display(kb, disp);
    lv_indev_set_group(kb, lv_group_get_default());

    return disp;
}

/**
 * The run loop of the SDL driver
 */
static void run_loop_sdl(void)
{
    uint32_t idle_time;

    /* Handle LVGL tasks */
    while(true) {
        /* Returns the time to the next timer execution */
        idle_time = lv_timer_handler();
        usleep(idle_time * 1000);
    }
}
#endif /*#if LV_USE_SDL*/
