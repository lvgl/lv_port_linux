/**
 * @file simulator_util.c
 *
 * Utility functions
 * Copyright (c) 2025 EDGEMTech Ltd.
 *
 * Based on the original file from the repo
 *
 * Author: EDGEMTech Ltd, Erik Tagirov (erik.tagirov@edgemtech.ch)
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

const char *getenv_default(const char *name, const char *default_val)
{
    return getenv(name) ? : default_val;
}


void die(const char *msg, ...)
{
    va_list args;

    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);

    exit(EXIT_FAILURE);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/
