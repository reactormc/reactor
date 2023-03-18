#ifndef BYTE_UTILS_H
#define BYTE_UTILS_H 1

#include <stdint.h>

typedef union uint8_to_uint16 {
    uint8_t chars[2];
    uint16_t short_value;
} uint8_to_uint16;

typedef union uint8_to_uint32 {
    uint8_t chars[4];
    uint32_t int_value;
} uint8_to_uint32;

typedef union uint8_to_uint64 {
    uint8_t chars[8];
    uint64_t long_value;
} uint8_to_uint64;

uint16_t read_uint16(char *bytes, int *offset);

#endif
