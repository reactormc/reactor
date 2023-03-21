/*
 * A simple logger facility that can be used in projects for debugging.
 */
#ifndef UTIL_LOGGER_H
#define UTIL_LOGGER_H 1

#include <stdio.h>
#include <stdarg.h>

int debug(char *, ...);

int debug_no_prefix(char *, ...);

#endif
