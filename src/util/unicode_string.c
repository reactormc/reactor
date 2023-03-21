#include "unicode_string.h"

#include <stdlib.h>
#include <string.h>

const int UNICODE_STRING_READ_SUCCESS = 0;
const int UNICODE_STRING_READ_ALLOC_FAILURE = 1;
const int UNICODE_STRING_READ_BUFFER_EXHAUSTED = 2;
const int UNICODE_STRING_READ_BUFFER_MAX_LENGTH_EXCEEDED = 3;

/* grow(void*, int*): int {{{1 */
int grow(UnicodeString *buffer, int *size) {
    *size += UNICODE_STRING_SIZE_INCR;

    UnicodeString tmp = realloc(*buffer, *size);
    if (!tmp) {
        return -1;
    }

    *buffer = tmp;
    return 0;
}
/* }}}1 */

/* read_lpus_from_bytes(char*, int, int): UnicodeString {{{1 */
int read_lpus_from_bytes(byte_buffer_ptr buffer, int max_length, UnicodeString *out) {
    int buffer_size = 0, buffer_len = INITIAL_UNICODE_STRING_SIZE;

    UnicodeString out_buffer = calloc(buffer_len, sizeof(UnicodeCharacter));
    if (!out_buffer) {
        return UNICODE_STRING_READ_ALLOC_FAILURE;
    }

    VarInt str_len = 0;
    if (buffer->read_varint(buffer, &str_len) == BYTE_BUFFER_READ_OUT_OF_BOUNDS) {
        free(out_buffer);
        return UNICODE_STRING_READ_BUFFER_EXHAUSTED;
    }

    if (str_len > buffer->remaining_length(buffer)) {
        free(out_buffer);
        buffer->rewind_last(buffer);
        return UNICODE_STRING_READ_BUFFER_EXHAUSTED;
    }

    if (str_len > max_length) {
        free(out_buffer);
        buffer->rewind_last(buffer);
        return UNICODE_STRING_READ_BUFFER_MAX_LENGTH_EXCEEDED;
    }

    int i, total_read = buffer->last_read_size;
    for (i = 0; i < str_len; i++) {
        UnicodeCharacter codepoint = (UnicodeCharacter) *buffer->bytes_at(buffer, buffer->read_offset);

        buffer_size++;
        if (buffer_size + 1 > buffer_len) {
            if (grow(&out_buffer, &buffer_len) != 0) {
                free(out_buffer);
                buffer->rewind(buffer, total_read);
                return UNICODE_STRING_READ_ALLOC_FAILURE;
            }
        }

        *(out_buffer + i) = codepoint;
        *(out_buffer + i + 1) = '\0';

        buffer->read_offset++;
        total_read++;
    }

    *out = out_buffer;

    return UNICODE_STRING_READ_SUCCESS;
}
/* }}}1 */

/* encode_ntls_to_lpus(const char*, int): UnicodeString {{{1
 *
 * Takes a null-terminated locale string (i.e. char* string) and a length thereof
 * and converts it to a length-prefixed unicode string as is used by Minecraft.
 */
UnicodeString encode_ntls_to_lpus(const char *string, int max_length) {
    int length = strlen(string);
    if (length > max_length) {
        return NULL;
    }

    int buffer_size = 0;

    uint8_t *buffer = calloc(max_length, sizeof(UnicodeCharacter));
    if (!buffer) {
        return NULL;
    }

    unsigned char bytes_written = 0;
    varint_encode(length, (char *) buffer, max_length, &bytes_written);

    memcpy(buffer + bytes_written, string, length - 1);

    return buffer;
}
/* }}}1 */

/* encode_ntus_to_lpus(const char*, int): char* {{{1
 * 
 * Takes a null-terminated unicode string and a length thereof and converts it to a
 * length-prefixed unicode string as is used by Minecraft.
 */
UnicodeString encode_ntus_to_lpus(UnicodeString ustring, int max_length) {
    size_t length = u8_strlen(ustring);
    if (length > max_length) {
        return NULL;
    }

    UnicodeString buffer = calloc(max_length, sizeof(UnicodeCharacter));
    if (!buffer) {
        return NULL;
    }

    unsigned char bytes_written = 0;
    varint_encode(length, (char *) buffer, max_length, &bytes_written);
    u8_cpy(buffer + bytes_written, ustring, length - 1);

    return buffer;
}
/* }}}1 */
