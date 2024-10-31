/**
 * @file fbdev.c
 *
 * The backend for the legacy framebuffer device
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
    const char *device = getenv_default("LV_LINUX_FBDEV_DEVICE", "/dev/fb0");
    lv_display_t * disp = lv_linux_fbdev_create();

    if (disp == NULL) {
        die("Failed to initialize FBDEV backend\n");
    }

    lv_linux_fbdev_set_file(disp, device);
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
