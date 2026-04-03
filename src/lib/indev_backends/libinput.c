/**
 * @file libinput.c
 *
 * Libinput driver
 *
 * Author: Malte Nilges (malte.nilges@glyn.de)
 * Based on evdev driver by EDGEMTech Ltd,
 *   Erik Tagirov (erik.tagirov@edgemtech.ch)
 *
 * Copyright (c) 2025 EDGEMTech Ltd.
 * Copyright (c) 2026 Malte Nilges
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "lvgl/lvgl.h"
#if LV_USE_LIBINPUT
#include "lvgl/src/core/lv_global.h"
#include "../backends.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void indev_deleted_cb(lv_event_t * e);
static void set_mouse_cursor_icon(lv_indev_t * indev, lv_display_t * display);
static lv_indev_t * init_pointer_libinput(lv_display_t * display);

/**********************
 *  STATIC VARIABLES
 **********************/

static char * backend_name = "LIBINPUT";

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*
 * Initialize the libinput driver
 *
 * @param backend the backend descriptor
 */
int backend_init_libinput(backend_t * backend)
{
    LV_ASSERT_NULL(backend);
    backend->handle->indev = malloc(sizeof(indev_backend_t));
    LV_ASSERT_NULL(backend->handle->indev);

    backend->handle->indev->init_indev = init_pointer_libinput;

    backend->name = backend_name;
    backend->type = BACKEND_INDEV;
    return 0;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/*
 * Remove cursor icon
 *
 * @description When the indev is deleted remove the mouse cursor icon
 * @note called by the LVGL libinput driver
 * @param e the deletion event
 */
static void indev_deleted_cb(lv_event_t * e)
{
    if(LV_GLOBAL_DEFAULT()->deinit_in_progress) return;
    lv_obj_t * cursor_obj = lv_event_get_user_data(e);
    lv_obj_delete(cursor_obj);
}

/*
 * Set cursor icon
 *
 * @description Enables a pointer (touchscreen/mouse) input device
 * @param display the display on which to create
 * @param indev the input device to set the cursor on
 */
static void set_mouse_cursor_icon(lv_indev_t * indev, lv_display_t * display)
{
    /* Set the cursor icon */
    LV_IMAGE_DECLARE(mouse_cursor_icon);
    lv_obj_t * cursor_obj = lv_image_create(lv_display_get_screen_active(display));
    lv_image_set_src(cursor_obj, &mouse_cursor_icon);
    lv_indev_set_cursor(indev, cursor_obj);

    /* delete the mouse cursor icon if the device is removed */
    lv_indev_add_event_cb(indev, indev_deleted_cb, LV_EVENT_DELETE, cursor_obj);

}

/*
 * Initialize a mouse pointer device
 *
 * Enables a pointer (touchscreen/mouse) input device
 *
 * If LV_LINUX_LIBINPUT_POINTER_DEVICE is not set, automatic libinput discovery will start
 *
 * @param display the LVGL display
 *
 * @return input device
 */
static lv_indev_t * init_pointer_libinput(lv_display_t * display)
{
    char * input_device = getenv("LV_LINUX_LIBINPUT_POINTER_DEVICE");

    if(input_device == NULL) {
        LV_LOG_USER("Using libinput automatic discovery.");
        const char *path = lv_libinput_find_dev(LV_LIBINPUT_CAPABILITY_TOUCH | LV_LIBINPUT_CAPABILITY_POINTER, true);
        if (path == NULL) {
            LV_LOG_USER("No touch device found.");
            return NULL;
        }
        input_device = path;
    }
    lv_indev_t *indev = lv_libinput_create(LV_INDEV_TYPE_POINTER, input_device);
    if (indev == NULL) {
        return NULL;
    }

    LV_LOG_USER("Using device at: %s", input_device);

    lv_indev_set_display(indev, display);

    set_mouse_cursor_icon(indev, display);
    return indev;

}
#endif /*#if LV_USE_LIBINPUT*/
