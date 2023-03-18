#include "byte_utils.h"

uint16_t read_uint16(char *bytes, int *offset) {
    uint8_to_uint16 converter;

    converter.chars[0] = *(bytes + *offset + 1);
    converter.chars[1] = *(bytes + *offset);

    *offset += 2;

    return converter.short_value;
}