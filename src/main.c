/**
 * @file main.c
 *
 * LVGL simulator for Unix-like systems
 *
 * Supports multiple backends
 *
 * - General cleanup and re-organization
 *   Author: EDGEMTech Ltd, erik.tagirov@edgemtech.ch
 */

#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"

#include "backends/interface.h"
#include "util.h"

uint16_t window_width;
uint16_t window_height;
bool fullscreen;
bool maximize;

static void configure_simulator(int argc, char **argv);

#if LV_USE_EVDEV
static void lv_linux_init_input_pointer(lv_display_t *disp)
{
    /* Enables a pointer (touchscreen/mouse) input device
     * Use 'evtest' to find the correct input device. /dev/input/by-id/ is recommended if possible
     * Use /dev/input/by-id/my-mouse-or-touchscreen or /dev/input/eventX
     */
    const char *input_device = getenv("LV_LINUX_EVDEV_POINTER_DEVICE");

    if (input_device == NULL) {
        die("please set the LV_LINUX_EVDEV_POINTER_DEVICE environment variable\n");
    }

    lv_indev_t *touch = lv_evdev_create(LV_INDEV_TYPE_POINTER, input_device);
    lv_indev_set_display(touch, disp);

    /* Set the cursor icon */
    LV_IMAGE_DECLARE(mouse_cursor_icon);
    lv_obj_t * cursor_obj = lv_image_create(lv_screen_active());
    lv_image_set_src(cursor_obj, &mouse_cursor_icon);
    lv_indev_set_cursor(touch, cursor_obj);
}
#endif

/*
 * Process command line arguments and environment
 * variables to configure the simulator
 */
static void configure_simulator(int argc, char **argv)
{

    int opt = 0;

    /* Default values */
    fullscreen = maximize = false;
    window_width = atoi(getenv("LV_SIM_WINDOW_WIDTH") ? : "800");
    window_height = atoi(getenv("LV_SIM_WINDOW_HEIGHT") ? : "480");

    /* Parse the command-line options. */
    while ((opt = getopt (argc, argv, "fmw:h:")) != -1) {
        switch (opt) {
        case 'f':
            fullscreen = true;
            if (LV_USE_WAYLAND == 0) {
                die("The SDL driver doesn't support fullscreen mode on start\n");
            }
            break;

        case 'm':
            maximize = true;
            if (LV_USE_WAYLAND == 0) {
                die("The SDL driver doesn't support maximized mode on start\n");
            }
            break;

        case 'w':
            window_width = atoi(optarg);
            break;

        case 'h':
            window_height = atoi(optarg);
            break;

        case ':':
            die("Option -%c requires an argument.\n", optopt);

        case '?':
            die("Unknown option -%c.\n", optopt);
        }
    }
}

int main(int argc, char **argv)
{

    configure_simulator(argc, argv);

    /* Initialize LVGL. */
    lv_init();

    /* Initialize the configured backend */
    lv_linux_disp_init();

    /*Create a Demo*/
    lv_demo_widgets();
    lv_demo_widgets_start_slideshow();

    lv_linux_run_loop();

    return 0;
}
