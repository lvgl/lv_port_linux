/**
 * @file wayland.c
 *
 * The wayland backend
 *
 */
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#include "lvgl/lvgl.h"
#include "settings.h"
#include "util.h"

/* Currently, the wayland driver calls lv_timer_handler internaly */
/* The wayland driver needs to be re-written to match the other backends */
void lv_linux_run_loop(void)
{

    bool completed;

    /* Handle LVGL tasks */
    while (true) {

        completed = lv_wayland_timer_handler();

        if (completed) {
            /* wait only if the cycle was completed */
            usleep(LV_DEF_REFR_PERIOD * 1000);
        }

        /* Run until the last window closes */
        if (!lv_wayland_window_is_open(NULL)) {
            break;
        }
    }

}

void lv_linux_disp_init(void)
{
    lv_display_t *disp;
    lv_group_t *g;

    disp = lv_wayland_window_create(window_width, window_height,
            "LVGL Simulator", NULL);

    if (disp == NULL) {
        die("Failed to initialize Wayland backend\n");
    }

    if (fullscreen) {
            lv_wayland_window_set_fullscreen(disp, fullscreen);
    } else if (maximize) {
            lv_wayland_window_set_maximized(disp, maximize);
    }

    g = lv_group_create();
    lv_group_set_default(g);
    lv_indev_set_group(lv_wayland_get_keyboard(disp), g);
    lv_indev_set_group(lv_wayland_get_pointeraxis(disp), g);

}
