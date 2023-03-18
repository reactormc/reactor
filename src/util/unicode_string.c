#include "unicode_string.h"
#include "util/varint.h"
#include <stdlib.h>
#include <string.h>
#include <unistr.h>
#include <unitypes.h>

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

/* read_lpus_from_bytes(char*, int, int): UnicodeString {{{1 */
UnicodeString read_lpus_from_bytes(char *bytes, int length, int *offset) {
    int buffer_size = 0, buffer_len = INITIAL_UNICODE_STRING_SIZE;

    UnicodeString buffer = calloc(buffer_len, sizeof(UnicodeCharacter));
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
        UnicodeCharacter codepoint = (UnicodeCharacter) *(bytes + *offset);

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
    varint_encode(length, (char*) buffer, max_length, &bytes_written);

    memcpy(buffer + bytes_written, string, length - 1);

    return buffer;
}
/* }}}1 */

/* encode_ntus_to_lpus(const char*, int): char* {{{1
 * 
 * Takes a null-terminated unicode string and a length thereof and converts it to a
 * length-prefixed unicode string as is used by Minecraft.
 */
UnicodeString encode_ntus_to_lpus(const UnicodeString ustring, int max_length) {
    int length = u8_strlen(ustring);
    if (length > max_length) {
        return NULL;
    }

    int buffer_size = 0;

    uint8_t *buffer = calloc(max_length, sizeof(UnicodeCharacter));
    if (!buffer) {
        return NULL;
    }

    unsigned char bytes_written = 0;
    varint_encode(length, (char*) buffer, max_length, &bytes_written);

    u8_cpy(buffer + bytes_written, ustring, length - 1);

    return buffer;
}
/* }}}1 */
