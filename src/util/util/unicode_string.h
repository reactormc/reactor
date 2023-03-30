#ifndef UTIL_UNICODE_STRING_H
#define UTIL_UNICODE_STRING_H 1

#include <unitypes.h>
#include <unistr.h>

#define INITIAL_UNICODE_STRING_SIZE 256
#define UNICODE_STRING_SIZE_INCR 512

uint8_t *encode_ntls_to_lpus(const char *string, int max_length);

uint8_t *encode_ntus_to_lpus(const uint8_t *string, int max_length);

#endif
