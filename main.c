#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lvgl/src/core/lv_global.h"

#if LV_USE_WAYLAND
#include "backends/interface.h"
#endif

uint16_t window_width;
uint16_t window_height;
bool fullscreen;
bool maximize;

static void configure_simulator(int argc, char **argv);

static const char *getenv_default(const char *name, const char *dflt)
{
    return getenv(name) ? : dflt;
}

#if LV_USE_EVDEV
static void indev_deleted_cb(lv_event_t * e)
{
    if(LV_GLOBAL_DEFAULT()->deinit_in_progress) return;
    lv_obj_t * cursor_obj = lv_event_get_user_data(e);
    lv_obj_delete(cursor_obj);
}

static void discovery_cb(lv_indev_t * indev, lv_evdev_type_t type, void * user_data)
{
    LV_LOG_USER("new '%s' device discovered", type == LV_EVDEV_TYPE_REL ? "REL" :
                                              type == LV_EVDEV_TYPE_ABS ? "ABS" :
                                              type == LV_EVDEV_TYPE_KEY ? "KEY" :
                                              "unknown");

    lv_display_t * disp = user_data;
    lv_indev_set_display(indev, disp);

    if(type == LV_EVDEV_TYPE_REL) {
        /* Set the cursor icon */
        LV_IMAGE_DECLARE(mouse_cursor_icon);
        lv_obj_t * cursor_obj = lv_image_create(lv_display_get_screen_active(disp));
        lv_image_set_src(cursor_obj, &mouse_cursor_icon);
        lv_indev_set_cursor(indev, cursor_obj);

        /* delete the mouse cursor icon if the device is removed */
        lv_indev_add_event_cb(indev, indev_deleted_cb, LV_EVENT_DELETE, cursor_obj);
    }
}

static void lv_linux_init_input_pointer(lv_display_t *disp)
{
    /* Enables a pointer (touchscreen/mouse) input device
     * Use 'evtest' to find the correct input device. /dev/input/by-id/ is recommended if possible
     * Use /dev/input/by-id/my-mouse-or-touchscreen or /dev/input/eventX
     * 
     * If LV_LINUX_EVDEV_POINTER_DEVICE is not set, automatic evdev disovery will start
     */
    const char *input_device = getenv("LV_LINUX_EVDEV_POINTER_DEVICE");

    if (input_device == NULL) {
        LV_LOG_USER("the LV_LINUX_EVDEV_POINTER_DEVICE environment variable is not set. using evdev automatic discovery.");
        lv_evdev_discovery_start(discovery_cb, disp);
        return;
    }

    lv_indev_t *touch = lv_evdev_create(LV_INDEV_TYPE_POINTER, input_device);
    lv_indev_set_display(touch, disp);

    /* Set the cursor icon */
    LV_IMAGE_DECLARE(mouse_cursor_icon);
    lv_obj_t * cursor_obj = lv_image_create(lv_display_get_screen_active(disp));
    lv_image_set_src(cursor_obj, &mouse_cursor_icon);
    lv_indev_set_cursor(touch, cursor_obj);
}
#endif

#if LV_USE_LINUX_FBDEV
static void lv_linux_disp_init(void)
{
    const char *device = getenv_default("LV_LINUX_FBDEV_DEVICE", "/dev/fb0");
    lv_display_t * disp = lv_linux_fbdev_create();

#if LV_USE_EVDEV
    lv_linux_init_input_pointer(disp);
#endif

    lv_linux_fbdev_set_file(disp, device);
}
#elif LV_USE_LINUX_DRM
static void lv_linux_disp_init(void)
{
    const char *device = getenv_default("LV_LINUX_DRM_CARD", "/dev/dri/card0");
    lv_display_t * disp = lv_linux_drm_create();

#if LV_USE_EVDEV
    lv_linux_init_input_pointer(disp);
#endif

    lv_linux_drm_set_file(disp, device, -1);
}
#elif LV_USE_SDL
static void lv_linux_disp_init(void)
{

    lv_sdl_window_create(window_width, window_height);
    
    lv_group_t * g = lv_group_create();
    lv_group_set_default(g);

    lv_sdl_mouse_create();
    lv_indev_t * mousewheel = lv_sdl_mousewheel_create();
    lv_indev_set_group(mousewheel, lv_group_get_default());

    lv_indev_t * keyboard = lv_sdl_keyboard_create();
    lv_indev_set_group(keyboard, lv_group_get_default());
}
#elif LV_USE_WAYLAND
    /* see backend/wayland.c */
#else
#error Unsupported configuration
#endif

#if LV_USE_WAYLAND == 0
void lv_linux_run_loop(void)
{
    uint32_t idle_time;

    /*Handle LVGL tasks*/
    while(1) {

        idle_time = lv_timer_handler(); /*Returns the time to the next timer execution*/
        usleep(idle_time * 1000);
    }
}
#endif

/*
 * Process command line arguments and environment
 * variables to configure the simulator
 */
static void configure_simulator(int argc, char **argv)
{

    int opt = 0;
    bool err = false;

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
                fprintf(stderr, "The SDL driver doesn't support fullscreen mode on start\n");
                exit(1);
            }
            break;
        case 'm':
            maximize = true;
            if (LV_USE_WAYLAND == 0) {
                fprintf(stderr, "The SDL driver doesn't support maximized mode on start\n");
                exit(1);
            }
            break;
        case 'w':
            window_width = atoi(optarg);
            break;
        case 'h':
            window_height = atoi(optarg);
            break;
        case ':':
            fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            exit(1);
        case '?':
            fprintf (stderr, "Unknown option -%c.\n", optopt);
            exit(1);
        }
    }
}

int main(int argc, char **argv)
{

    configure_simulator(argc, argv);

    /* Initialize LVGL. */
    lv_init();

    /* Initialize the configured backend SDL2, FBDEV, libDRM or wayland */
    lv_linux_disp_init();

    /*Create a Demo*/
    lv_demo_widgets();
    lv_demo_widgets_start_slideshow();

    lv_linux_run_loop();

    return 0;
}
