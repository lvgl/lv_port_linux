/**
 * @file util.c
 *
 * Utility functions
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>


/**
 * @description Wrapper around getenv(3), allowing to set a default value
 * @param name The name of the environment variable
 * @param dflt The default value to set if the variable is not present.
 * @return default value or value of environment variable.
 */
const char *getenv_default(const char *name, const char *dflt)
{
    return getenv(name) ? : dflt;
}


/**
 * @description Centralized exit point, called due to an error
 * @param msg The message to display on stderr before killing the program
 * @param ... Values for the format string.
 */
void die(const char *msg, ...)
{
    va_list args;

    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);

    exit(EXIT_FAILURE);

}
