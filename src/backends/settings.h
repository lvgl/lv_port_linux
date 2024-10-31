/**
 * @file settings.h
 *
 * Must be included in each backend files
 * Declares variables that are defined in the main file
 * Those variables contain simulator settings, such as screen size etc
 */

#include <stdbool.h>
#include <stdint.h>

/* Catch configuration errors at compile time */
#if LV_USE_SDL == 0 && \
    LV_USE_WAYLAND == 0 && \
    LV_USE_LINUX_DRM == 0 && \
    LV_USE_LINUX_FBDEV == 0

#error Unsupported configuration - Please select at least one backend in lv_conf.h

#endif

/* Settings defined in main.c */

extern uint16_t window_width;
extern uint16_t window_height;

extern bool fullscreen;
extern bool maximize;


