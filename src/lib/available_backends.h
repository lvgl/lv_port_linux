/*******************************************************************
 *
 * Declaractions for supported backends for both
 * graphics and input devices
 * This is ONLY included in lv_driver_backends.c
 *
 * To add support for a new driver backend add the declaration
 * and append an entry to the available_backends array
 *
 * Copyright (c) 2025 EDGEMTech Ltd.
 * Author: EDGEMTech Ltd, Erik Tagirov (erik.tagirov@edgemtech.ch)
 *
 ******************************************************************/

#define BACKEND_CNT 10

/* Catch configuration errors at compile time - checks if no backend was selected */
#if LV_USE_SDL == 0 && \
    LV_USE_WAYLAND == 0 && \
    LV_USE_LINUX_DRM == 0 && \
    LV_USE_OPENGLES == 0 && \
    LV_USE_X11 == 0 && \
    LV_USE_LINUX_FBDEV == 0

#error Unsupported configuration - Please select at least one graphics backend in lv_conf.h
#endif

/* Graphics backends */
int backend_init_fbdev(backend_t *backend);
int backend_init_drm(backend_t *backend);
int backend_init_sdl(backend_t *backend);
int backend_init_glfw3(backend_t *backend);
int backend_init_wayland(backend_t *backend);
int backend_init_x11(backend_t *backend);

/* Input device driver backends */
int backend_init_evdev(backend_t *backend);

/* The default backend is the one that will end up at index 0 */
backend_init_t available_backends[BACKEND_CNT + 1] = {

#if LV_USE_LINUX_FBDEV
    backend_init_fbdev,
#endif

#if LV_USE_LINUX_DRM
    backend_init_drm,
#endif

#if LV_USE_SDL
    backend_init_sdl,
#endif

#if LV_USE_WAYLAND
    backend_init_wayland,
#endif

#if LV_USE_X11
    backend_init_x11,
#endif

#if LV_USE_OPENGLES
    backend_init_glfw3,
#endif

#if LV_USE_EVDEV
    backend_init_evdev,
#endif
    NULL    /* Sentinel */
};

