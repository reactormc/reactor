#ifndef UTIL_UNICODE_STRING_H
#define UTIL_UNICODE_STRING_H 1

#include "byte_buffer.h"

#include <unitypes.h>
#include <unistr.h>

#define INITIAL_UNICODE_STRING_SIZE 256
#define UNICODE_STRING_SIZE_INCR 512

extern const int UNICODE_STRING_READ_SUCCESS; //0
extern const int UNICODE_STRING_READ_ALLOC_FAILURE; //1
extern const int UNICODE_STRING_READ_BUFFER_EXHAUSTED; //2
extern const int UNICODE_STRING_READ_BUFFER_MAX_LENGTH_EXCEEDED; //3

typedef ucs4_t UnicodeCharacter;
typedef uint8_t *UnicodeString;

int read_lpus_from_bytes(byte_buffer_ptr buffer, int max_length, UnicodeString *out);

UnicodeString encode_ntls_to_lpus(const char *string, int max_length);

UnicodeString encode_ntus_to_lpus(UnicodeString string, int max_length);

#endif
