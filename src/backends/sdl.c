/**
 * @file sdl.c
 *
 * The SDL backend
 *
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lvgl/lvgl.h"
#include "settings.h"

void lv_linux_disp_init(void)
{
    lv_display_t *disp;

    disp = lv_sdl_window_create(window_width, window_height);

    if (disp == NULL) {
        die("Failed to initialize the SDL backend\n");
    }
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
