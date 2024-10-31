/**
 * @file util.c
 *
 * Utility functions
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

const char *getenv_default(const char *name, const char *dflt)
{
    return getenv(name) ? : dflt;
}


void die(const char *msg, ...)
{
    va_list args;

    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);

    exit(EXIT_FAILURE);

}
