#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <poll.h>

#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"

static uint16_t window_width;
static uint16_t window_height;
bool fullscreen;
bool maximize;

void default_run_loop(void);
void wayland_run_loop(void);


static const char *getenv_default(const char *name, const char *dflt)
{
    return getenv(name) ? : dflt;
}

#if LV_USE_LINUX_FBDEV
static void lv_linux_disp_init(void)
{
    const char *device = getenv_default("LV_LINUX_FBDEV_DEVICE", "/dev/fb0");
    lv_display_t * disp = lv_linux_fbdev_create();

    lv_linux_fbdev_set_file(disp, device);
}
#elif LV_USE_LINUX_DRM
static void lv_linux_disp_init(void)
{
    const char *device = getenv_default("LV_LINUX_DRM_CARD", "/dev/dri/card0");
    lv_display_t * disp = lv_linux_drm_create();

    lv_linux_drm_set_file(disp, device, -1);
}
#elif LV_USE_SDL
static void lv_linux_disp_init(void)
{

    lv_sdl_window_create(window_width, window_height);

}
#elif LV_USE_WAYLAND
static void lv_linux_disp_init(void)
{
    lv_display_t *disp;
    lv_group_t *g;

    lv_wayland_init();
    disp = lv_wayland_window_create(window_width, window_height, "LVGL Simulator", NULL);

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
#else
#error Unsupported configuration
#endif

#if !LV_USE_WAYLAND

void default_run_loop(void)
{
    /*Handle LVGL tasks*/
    while(1) {
        lv_timer_handler();
        usleep(5000);
    }
}


#else

void wayland_run_loop(void)
{
    struct pollfd pfd;
    int sleep_wait = 0;
    uint32_t time_till_next;
    uint8_t tickless = 0;

    pfd.fd = lv_wayland_get_fd();
    pfd.events = POLLIN;

    /* Handle LVGL tasks */
    while (1) {

        if (tickless == 0 && sleep_wait > 0) {
            lv_tick_inc(sleep_wait);
        }

        /* Handle any Wayland/LVGL timers/events */
        time_till_next = lv_wayland_timer_handler();

        /* Run until the last window closes */
        if (!lv_wayland_window_is_open(NULL)) {
            break;
        }

        /* Wait for something for an event to happen */
        if (time_till_next == LV_NO_TIMER_READY) {
            sleep_wait = -1;
        } else if (time_till_next > INT_MAX) {
            sleep_wait = INT_MAX;
        } else {
            sleep_wait = time_till_next;
        }

        while ((poll(&pfd, 1, sleep_wait) < 0) && (errno == EINTR));
    }
}

#endif

int main(int argc, char **argv)
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
            if (LV_USE_SDL) {
                fprintf(stderr, "The SDL driver doesn't support fullscreen mode on start\n");
                exit(1);
            }
            break;
        case 'm':
            maximize = true;
            if (LV_USE_SDL) {
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
            return EXIT_FAILURE;
        case '?':
            fprintf (stderr, "Unknown option -%c.\n", optopt);
            return EXIT_FAILURE;
        default:
            return EXIT_FAILURE;
        }
    }

    /* Initialize LVGL. */
    lv_init();

    /* Initialize the configured backend SDL2, FBDEV, libDRM or wayland */
    lv_linux_disp_init();

    /*Create a Demo*/
    lv_demo_widgets();
    lv_demo_widgets_start_slideshow();

    if (LV_USE_WAYLAND) {
        wayland_run_loop();
    } else {
        default_run_loop();
    }

    return 0;
}
