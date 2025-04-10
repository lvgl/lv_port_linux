/**
 * @file glfw3.c
 *
 * The GLFW3 backend
 *
 * Based on the original file from the repository
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
#if LV_USE_OPENGLES
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
static void run_loop_glfw3(void);
static lv_display_t *init_glfw3(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static char *backend_name = "GLFW";

/**********************
 *  EXTERNAL VARIABLES
 **********************/
extern simulator_settings_t settings;

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
int backend_init_glfw3(backend_t *backend)
{

    LV_ASSERT_NULL(backend);
    backend->handle->display = malloc(sizeof(display_backend_t));

    LV_ASSERT_NULL(backend->handle->display);

    backend->handle->display->init_display = init_glfw3;
    backend->handle->display->run_loop = run_loop_glfw3;
    backend->name = backend_name;
    backend->type = BACKEND_DISPLAY;

    return 0;

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Initialize the GLFW3 display driver
 *
 * @note GLFW3 provides OpenGL but also handles inputs devices
 * @return the LVGL display
 */
static lv_display_t *init_glfw3(void)
{
    lv_glfw_texture_t *window_texture;
    lv_indev_t *mouse;
    lv_display_t *disp_texture;
    uint32_t disp_texture_id;
    lv_obj_t *cursor_obj;

    /* create a window and initialize OpenGL */
    lv_glfw_window_t * window = lv_glfw_window_create(
            settings.window_width, settings.window_height, true);

    /* create a display that flushes to a texture */
    disp_texture = lv_opengles_texture_create(
            settings.window_width, settings.window_height);
    lv_display_set_default(disp_texture);

    /* add the texture to the window */
    disp_texture_id = lv_opengles_texture_get_texture_id(disp_texture);
    window_texture = lv_glfw_window_add_texture(window, disp_texture_id,
            settings.window_width, settings.window_height);

    /* get the mouse indev of the window texture */
    mouse = lv_glfw_texture_get_mouse_indev(window_texture);

    /* add a cursor to the mouse indev */
    LV_IMAGE_DECLARE(mouse_cursor_icon);
    cursor_obj = lv_image_create(lv_screen_active());
    lv_image_set_src(cursor_obj, &mouse_cursor_icon);
    lv_indev_set_cursor(mouse, cursor_obj);

    return disp_texture;
}

/**
 * The run loop of the GLFW3 driver
 */
void run_loop_glfw3(void)
{
    uint32_t idle_time;

    /* Handle LVGL tasks */
    while (true) {

        /* Returns the time to the next timer execution */
        idle_time = lv_timer_handler();
        usleep(idle_time * 1000);
    }
}

#endif /*#if LV_USE_OPENGLES*/
