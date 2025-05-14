/*******************************************************************
 *
 * main.c - LVGL simulator for GNU/Linux
 *
 * Based on the original file from the repository
 *
 * @note eventually this file won't contain a main function and will
 * become a library supporting all major operating systems
 *
 * To see how each driver is initialized check the
 * 'src/lib/display_backends' directory
 *
 * - Clean up
 * - Support for multiple backends at once
 *   2025 EDGEMTech Ltd.
 *
 * Author: EDGEMTech Ltd, Erik Tagirov (erik.tagirov@edgemtech.ch)
 *
 ******************************************************************/
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"

#include "lvgl/examples/lv_examples.h"
#include "smiths_fg.c"
#include "smiths_bg.c"
#include "gauge.c"

#include "src/lib/driver_backends.h"
#include "src/lib/simulator_util.h"
#include "src/lib/simulator_settings.h"

/* Internal functions */
static void configure_simulator(int argc, char **argv);
static void print_lvgl_version(void);
static void print_usage(void);

/* contains the name of the selected backend if user
 * has specified one on the command line */
static char *selected_backend;

/* Global simulator settings, defined in lv_linux_backend.c */
extern simulator_settings_t settings;

void draw_line(void)
{
    /*Create an array for the points of the line*/
    // static lv_point_precise_t line_points[] = { {5, 5}, {70, 70}, {120, 10}, {180, 60}, {240, 10} };
    static lv_point_precise_t line_points[] = { {5, 5}, {90, 90} };

    /*Create style*/
    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 8);
    lv_style_set_line_color(&style_line, lv_color_white());
    lv_style_set_line_rounded(&style_line, true);

    /*Create a line and apply the new style*/
    lv_obj_t * line1;
    line1 = lv_line_create(lv_scr_act());
    lv_line_set_points(line1, line_points, 2);     /*Set the points*/
    lv_obj_add_style(line1, &style_line, 0);
    // lv_obj_center(line1);
    lv_obj_align(line1, LV_ALIGN_TOP_LEFT, 250, 200);
}

LV_IMG_DECLARE(gauge);
lv_obj_t * draw_gauge_orig(lv_obj_t * parent){
    lv_obj_t * img_src = lv_image_create(parent); /*Crate an image object*/
    lv_image_set_src(img_src, &gauge);
    lv_image_set_inner_align(img_src, LV_IMAGE_ALIGN_CENTER);
    lv_obj_align(img_src, LV_ALIGN_TOP_LEFT, 30, 20);
    lv_image_set_scale(img_src, 245);
    return img_src;
}


//50,228
LV_IMG_DECLARE(smiths_bg);
lv_obj_t * draw_gauge_bg(lv_obj_t * parent){
    lv_obj_t * img_src = lv_image_create(parent); /*Crate an image object*/
    lv_image_set_src(img_src, &smiths_bg);
    lv_image_set_inner_align(img_src, LV_IMAGE_ALIGN_CENTER);
    lv_obj_align(img_src, LV_ALIGN_TOP_LEFT, 50, 228);
    // lv_image_set_scale(img_src, 170);
    return img_src;
}

LV_IMG_DECLARE(smiths_fg);
lv_obj_t * draw_gauge(void){
    lv_obj_t * img_src = lv_image_create(lv_scr_act()); /*Crate an image object*/
    lv_image_set_src(img_src, &smiths_fg);
    lv_image_set_inner_align(img_src, LV_IMAGE_ALIGN_CENTER);
    lv_obj_align(img_src, LV_ALIGN_TOP_LEFT, 0, 0);
    // lv_image_set_scale(img_src, 170);
    return img_src;
}

void say_hello(){
    /*Create a white label, set its text and align it to the center*/
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello world");
    lv_obj_set_style_text_color(label, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 80, 0);
}

/**
 * @brief Print LVGL version
 */
static void print_lvgl_version(void)
{
    fprintf(stdout, "%d.%d.%d-%s\n",
            LVGL_VERSION_MAJOR,
            LVGL_VERSION_MINOR,
            LVGL_VERSION_PATCH,
            LVGL_VERSION_INFO);
}

/**
 * @brief Print usage information
 */
static void print_usage(void)
{
    fprintf(stdout, "\nlvglsim [-V] [-B] [-b backend_name] [-W window_width] [-H window_height]\n\n");
    fprintf(stdout, "-V print LVGL version\n");
    fprintf(stdout, "-B list supported backends\n");
}

/**
 * @brief Configure simulator
 * @description process arguments recieved by the program to select
 * appropriate options
 * @param argc the count of arguments in argv
 * @param argv The arguments
 */
static void configure_simulator(int argc, char **argv)
{
    int opt = 0;
    char *backend_name;

    selected_backend = NULL;
    driver_backends_register();

    /* Default values */
    settings.window_width = atoi(getenv("LV_SIM_WINDOW_WIDTH") ? : "800");
    settings.window_height = atoi(getenv("LV_SIM_WINDOW_HEIGHT") ? : "480");

    /* Parse the command-line options. */
    while ((opt = getopt (argc, argv, "b:fmW:H:BVh")) != -1) {
        switch (opt) {
        case 'h':
            print_usage();
            exit(EXIT_SUCCESS);
            break;
        case 'V':
            print_lvgl_version();
            exit(EXIT_SUCCESS);
            break;
        case 'B':
            driver_backends_print_supported();
            exit(EXIT_SUCCESS);
            break;
        case 'b':
            if (driver_backends_is_supported(optarg) == 0) {
                die("error no such backend: %s\n", optarg);
            }
            selected_backend = strdup(optarg);
            break;
        case 'W':
            settings.window_width = atoi(optarg);
            break;
        case 'H':
            settings.window_height = atoi(optarg);
            break;
        case ':':
            print_usage();
            die("Option -%c requires an argument.\n", optopt);
            break;
        case '?':
            print_usage();
            die("Unknown option -%c.\n", optopt);
        }
    }
}

/**
 * @brief entry point
 * @description start a demo
 * @param argc the count of arguments in argv
 * @param argv The arguments
 */
int main(int argc, char **argv)
{

    configure_simulator(argc, argv);

    /* Initialize LVGL. */
    lv_init();

    /* Initialize the configured backend */
    if (driver_backends_init_backend(selected_backend) == -1) {
        die("Failed to initialize display backend");
    }

    /* Enable for EVDEV support */
#if LV_USE_EVDEV
    if (driver_backends_init_backend("EVDEV") == -1) {
        die("Failed to initialize evdev");
    }
#endif


   /*Change the active screen's background color*/
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);


    /*Create a Demo*/
    // lv_demo_widgets();
    // lv_demo_widgets_start_slideshow();
    // lv_example_scale_3();
    // lv_example_scale_6();

    // say_hello();
    draw_line();
    lv_obj_t * guage_obj = draw_gauge();
    lv_obj_t * guage_obj_bg = draw_gauge_orig(lv_screen_active());
    // lv_obj_t * guage_obj_bg = draw_gauge_bg(lv_screen_active());
    lv_obj_move_background(guage_obj_bg);
    // lv_example_canvas_5();
    // lv_example_scale_3(guage_obj);

    /* Enter the run loop of the selected backend */
    driver_backends_run_loop();

    return 0;
}
