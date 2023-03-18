#include "unicode_string.h"
#include "util/varint.h"
#include <stdlib.h>
#include <string.h>

/* grow(void*, int*): int {{{1 */
int grow(void *buffer, int *size) {
    *size += UNICODE_STRING_SIZE_INCR;

    void *tmp = realloc(buffer, *size);
    if (!tmp) {
        return -1;
    }

    buffer = tmp;
    return 0;
}
/* }}}1 */

/* read_unicode_string(char*, int, int): UnicodeString {{{1 */
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
        uint8_t codepoint = (uint8_t) *(bytes + *offset);

        buffer_size++;
        if (buffer_size + 1 > buffer_len) {
            if (grow(buffer, &buffer_len) != 0) {
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

/* encode_to_unicode_string(const char*, int): char* {{{1 */
char *encode_to_unicode_string(const char *string, int max_length) {
    int length = strlen(string);
    if (length > max_length) {
        return NULL;
    }

    int buffer_size = 0;

    char *buffer = calloc(max_length, sizeof(char));
    if (!buffer) {
        return NULL;
    }

    unsigned char bytes_written = 0;
    varint_encode(length, buffer, max_length, &bytes_written);

    int i, off;
    for (i = 0, off = bytes_written; i < length; i++, off++) {
        buffer_size++;

        if (buffer_size > max_length) {
            // string gets truncated
            break;
        }

        *(buffer + off) = *(string + i);
    }

    return buffer;
}
/* }}}1 */
