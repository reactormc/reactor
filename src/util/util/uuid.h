#ifndef UTIL_UUID4_H
#define UTIL_UUID4_H 1

#include <stdint.h>

typedef union uuid_t {
    uint64_t word[2];
    uint8_t bit[16];
} uuid_t;

int uuid_to_string(uuid_t *uuid, char **out, uint8_t dashed);
int uuid_to_dashed_string(uuid_t *uuid, char **out);
int uuid_to_dashless_string(uuid_t *uuid, char** out);

#endif
