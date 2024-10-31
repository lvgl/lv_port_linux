/**
 * @file drm.c
 *
 * The libdrm backend
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
    const char *device = getenv_default("LV_LINUX_DRM_CARD", "/dev/dri/card0");
    lv_display_t * disp = lv_linux_drm_create();

    if (disp == NULL) {
        die("Failed to initialize DRM backend\n");
    }

    lv_linux_drm_set_file(disp, device, -1);
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
