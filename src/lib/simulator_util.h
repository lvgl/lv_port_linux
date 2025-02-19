/**
 * @file simulator_util.h
 *
 * simulator_util.h - Header file for the utility functions
 * used by the simulator
 *
 * Copyright (c) 2025 EDGEMTech Ltd.
 *
 * Author: EDGEMTech Ltd, Erik Tagirov (erik.tagirov@edgemtech.ch)
 */

#ifndef SIMULATOR_UTIL_H
#define SIMULATOR_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdarg.h>


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @description Wrapper around getenv(3), allowing to set a default value
 * @param name The name of the environment variable
 * @param dflt The default value to set if the variable is not present.
 * @return default value or value of environment variable.
 */
const char *getenv_default(const char *name, const char *default_val);


/**
 * @description Centralized exit point, called due to an error
 * @param msg The message to display on stderr before killing the program
 * @param ... Values for the format string.
 */
void die(const char *msg, ...);

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*SIMULATOR_UTIL_H*/
