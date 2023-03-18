#ifndef USTRING_H
#define USTRING_H 1

#include <unitypes.h>
#include <unistr.h>

#define INITIAL_UNICODE_STRING_SIZE 256
#define UNICODE_STRING_SIZE_INCR 512

typedef ucs4_t UnicodeCharacter;
typedef uint8_t *UnicodeString;

UnicodeString read_lpus_from_bytes(char *bytes, int length, int *offset);
UnicodeString encode_ntls_to_lpus(const char *string, int max_length);
UnicodeString encode_ntus_to_lpus(const UnicodeString string, int max_length);

#endif
