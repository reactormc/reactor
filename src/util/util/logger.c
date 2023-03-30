/*
 * A simple logger facility that can be used in projects for debugging.
 */

#include "logger.h"

/* debug(char*, ...) {{{1 */
int debug(char *msg, ...) {
#ifdef DEBUG
    int ret = 0;
    va_list argp;
    va_start(argp, msg);
    fprintf(stdout, "DEBUG: ");
    ret = vfprintf(stdout, msg, argp);
    va_end(argp);
    return ret;
#else
    return 0;
#endif
}
/* }}}1 */

/* debug_no_prefix(char*, ...) {{{1 */
int debug_no_prefix(char *msg, ...) {
#ifdef DEBUG
    int ret = 0;
    va_list argp;
    va_start(argp, msg);
    ret = vfprintf(stdout, msg, argp);
    va_end(argp);
    return ret;
#else
    return 0;
#endif
}
/* }}}1 */
