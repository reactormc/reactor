#ifndef DATATYPES_H
#define DATATYPES_H 1

#include <unitypes.h>
#include <unistr.h>

#define VARINT_MAX_LEN 5
#define VARLONG_MAX_LEN 10
typedef unsigned long long VarInt;

typedef ucs4_t UnicodeCharacter;
typedef uint8_t *UnicodeString;

#endif
