/**
 * @file util.h
 *
 * Prototypes of utility functions
 *
 */
#include <stdarg.h>

const char *getenv_default(const char *name, const char *dflt);

void die(const char *msg, ...);
