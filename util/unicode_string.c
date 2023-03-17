#include "unicode_string.h"
#include "../include/varint.h"
#include <netinet/in.h>
#include <stdlib.h>

/* _grow(UnicodeString, int*): int {{{1 */
int _grow(UnicodeString *string, int *size) {
    *size += UNICODE_STRING_SIZE_INCR;

    UnicodeString tmp = realloc(*string, *size);
    if (!tmp) {
        return -1;
    }

    *string = tmp;
    return 0;
}
/* }}}1 */

/* read_unicode_string(char*, int, int) {{{1 */
UnicodeString read_unicode_string(char *bytes, int length, int *offset) {
    int buffer_size = 0, buffer_len = INITIAL_UNICODE_STRING_SIZE;

    UnicodeString buffer = calloc(buffer_len, sizeof(uint8_t));
    if (!buffer) {
        return NULL;
    }

    int remaining_length = length - *offset;
    if (remaining_length <= 0) {
        return NULL;
    }

    unsigned char bytes_read = 0;
    int str_len = varint_decode(bytes + *offset, remaining_length, &bytes_read);
    *offset += bytes_read;

    if (str_len > remaining_length) {
        return NULL;
    }

    int i;
    for (i = 0; i < str_len; i++, (*offset)++) {
        uint8_t codepoint = (uint8_t) ntohl(*(bytes + *offset));

        buffer_size++;
        if (buffer_size + 1 > buffer_len) {
            if (_grow(&buffer, &buffer_len) != 0) {
                free(buffer);
                return NULL;
            }
        }

        *(buffer + i) = codepoint;
        *(buffer + i + 1) = '\0';
    }

    return buffer;
}
/* }}}1 */
