/**
 * @file main.c
 *
 * LVGL example application using LVGL on Unix-like
 * operating systems - It is used to test if the build works
 * with dynamic linking and finding the library on the system
 *
 * NOTE: before compiling this program be sure
 * to first compile & install LVGL on your system
 * detailed instructions are available in the README.md
 *
 * Author: EDGEMTech Ltd, erik.tagirov@edgemtech.ch
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <lvgl.h>
#include <lvgl/demos/lv_demos.h>
#include <lvgl/driver_backends.h>
#include <lvgl/simulator_settings.h>

extern simulator_settings_t settings;

int main(int argc, char **argv)
{

    /* Initialize LVGL. */
    lv_init();

    settings.window_width = 800;
    settings.window_height = 480;

    /* Initialize the configured default backend */
    driver_backends_register();
    if (driver_backends_init_backend("WAYLAND") == -1) {
        fprintf(stderr, "Failed to initialize wayland backend\n");
        exit(EXIT_FAILURE);
    }

    /*Create a Demo*/
    lv_demo_widgets();
    lv_demo_widgets_start_slideshow();

    /* Enter the run loop - does not return */
    driver_backends_run_loop();

    return 0;
}
