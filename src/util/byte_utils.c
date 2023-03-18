#include "byte_utils.h"

uint16_t read_uint16(char *bytes, int *offset) {
    uint8_to_uint16 converter;

    converter.chars[0] = *(bytes + *offset + 1);
    converter.chars[1] = *(bytes + *offset);

    *offset += 2;

    return converter.short_value;
}

uint32_t read_uint32(char *bytes, int *offset) {
    uint8_to_uint32 converter;

    converter.chars[0] = *(bytes + *offset + 3);
    converter.chars[1] = *(bytes + *offset + 2);
    converter.chars[2] = *(bytes + *offset + 1);
    converter.chars[3] = *(bytes + *offset);

    *offset += 4;

    return converter.int_value;
}

uint64_t read_uint64(char *bytes, int *offset) {
    uint8_to_uint64 converter;

    converter.chars[0] = *(bytes + *offset + 7);
    converter.chars[1] = *(bytes + *offset + 6);
    converter.chars[2] = *(bytes + *offset + 5);
    converter.chars[3] = *(bytes + *offset + 4);
    converter.chars[4] = *(bytes + *offset + 3);
    converter.chars[5] = *(bytes + *offset + 2);
    converter.chars[6] = *(bytes + *offset + 1);
    converter.chars[7] = *(bytes + *offset);

    *offset += 8;

    return converter.long_value;
}