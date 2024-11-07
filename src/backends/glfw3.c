/**
 * @file glfw3.c
 *
 * The backend for the glfw3 library
 *
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lvgl/lvgl.h"
#include "settings.h"
#include "util.h"

void lv_linux_disp_init(void)
{
    lv_glfw_texture_t *window_texture;
    lv_indev_t *mouse;
    lv_display_t *disp_texture;
    uint32_t disp_texture_id;
    lv_obj_t *cursor_obj;

    /* create a window and initialize OpenGL */
    lv_glfw_window_t * window = lv_glfw_window_create(window_width, window_height, true);

    /* create a display that flushes to a texture */
    disp_texture = lv_opengles_texture_create(window_width, window_height);
    lv_display_set_default(disp_texture);

    /* add the texture to the window */
    disp_texture_id = lv_opengles_texture_get_texture_id(disp_texture);
    window_texture = lv_glfw_window_add_texture(window, disp_texture_id,
            window_width, window_height);

    /* get the mouse indev of the window texture */
    mouse = lv_glfw_texture_get_mouse_indev(window_texture);

    /* add a cursor to the mouse indev */
    LV_IMAGE_DECLARE(mouse_cursor_icon);
    cursor_obj = lv_image_create(lv_screen_active());
    lv_image_set_src(cursor_obj, &mouse_cursor_icon);
    lv_indev_set_cursor(mouse, cursor_obj);
}

void lv_linux_run_loop(void)
{
    uint32_t idle_time;

    /* Handle LVGL tasks */
    while (true) {

        idle_time = lv_timer_handler(); /* Returns the time to the next timer execution */
        usleep(idle_time * 1000);
    }
}
