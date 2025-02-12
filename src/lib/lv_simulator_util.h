/*******************************************************************
 *
 * lv_simulator_util.h - Header file for the utility functions
 *
 * Copyright (c) 2025 EDGEMTech Ltd.
 *
 * Author: EDGEMTech Ltd, Erik Tagirov (erik.tagirov@edgemtech.ch)
 *
 ******************************************************************/

#include <stdarg.h>

const char *getenv_default(const char *name, const char *dflt);

void die(const char *msg, ...);
