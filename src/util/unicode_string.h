#ifndef USTRING_H
#define USTRING_H 1

#include "../packet/datatypes.h"

#define INITIAL_UNICODE_STRING_SIZE 256
#define UNICODE_STRING_SIZE_INCR 512

UnicodeString read_unicode_string(char *bytes, int length, int *offset);

char *encode_to_unicode_string(const char *regular, int max_length);

#endif
