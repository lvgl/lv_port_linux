/*******************************************************************
 *
 * simulator.c - LVGL simulator for GNU/Linux
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

#include "lv_driver_backends.h"
#include "lv_simulator_util.h"
#include "lv_simulator_settings.h"

/* Internal functions */
static void configure_simulator(int argc, char **argv);
static int backend_exists(char **supported_backends, char *backend_name);
static void pr_supported_backends(char **supported_backends);
static void pr_lvgl_version(void);

/* contains the name of the selected backend if user
 * has specified one on the command line */
static char *selected_backend;

/* Global simulator settings, defined in lv_linux_backend.c */
extern simulator_settings_t settings;


/**
 * @brief Print supported backends
 * @description Prints a list of supported backends
 *
 * @param supported_backends array of strings containing a list of supported backends
 */
static void pr_supported_backends(char **supported_backends)
{
    int i;
    char *backend;

    backend = *supported_backends++;

    fprintf(stdout, "Default backend: %s\n", backend);
    fprintf(stdout, "Supported backends: ");

    while (backend != NULL) {
        fprintf(stdout, "%s ", backend);
        backend = *supported_backends++;
    }

    fprintf(stdout, "\n");

}

/**
 * @brief Checks if a backend exists in the available backends list
 * @param supported_backends array of strings containing a list of supported backends
 * @param backend_name the backend name to check
 * @return 1 if exists, 0 if doesn't exist
 */
static int backend_exists(char **supported_backends, char *backend_name)
{
    char *b;
    char c;
    char *name = backend_name;

    while ((c = *backend_name) != '\0') {
        *backend_name = toupper(c);
        *backend_name++;
    }

    while ((b = *supported_backends++) != NULL) {
        if (strcmp(b, name) == 0) {
            return 1;
        }
    }

    return 0;
}

/**
 * @brief Print LVGL version
 */
static void pr_lvgl_version(void)
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
static void pr_usage(void)
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
    char **supported_backends;

    supported_backends = lv_driver_backends_get_supported();

    /* Default values */
    settings.window_width = atoi(getenv("LV_SIM_WINDOW_WIDTH") ? : "800");
    settings.window_height = atoi(getenv("LV_SIM_WINDOW_HEIGHT") ? : "480");

    /* Parse the command-line options. */
    while ((opt = getopt (argc, argv, "b:fmW:H:BVh")) != -1) {
        switch (opt) {
        case 'h':
            pr_usage();
            exit(EXIT_SUCCESS);
            break;
        case 'V':
            pr_lvgl_version();
            exit(EXIT_SUCCESS);
            break;
        case 'B':
            pr_supported_backends(supported_backends);
            exit(EXIT_SUCCESS);
            break;
        case 'b':
            if (backend_exists(supported_backends, optarg) == 0) {
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
            pr_usage();
            die("Option -%c requires an argument.\n", optopt);
            break;
        case '?':
            pr_usage();
            die("Unknown option -%c.\n", optopt);
        }
    }

    if (selected_backend == NULL) {
        /* Select the default backend */
        selected_backend = *supported_backends;
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
    if (lv_driver_backends_init_backend(selected_backend) == -1) {
        die("Failed to initialize display backend");
    }

    /* Enable for EVDEV support */
#if 0
    if (lv_driver_backends_init_backend("EVDEV") == -1) {
        die("Failed to initialize evdev");
    }
#endif

    /*Create a Demo*/
    lv_demo_widgets();
    lv_demo_widgets_start_slideshow();

    /* Enter the run loop of the selected backend */
    lv_driver_backends_run_loop();

    return 0;
}
