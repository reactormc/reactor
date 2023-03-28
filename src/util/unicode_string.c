#include "unicode_string.h"
#include "varint.h"

#include <stdlib.h>
#include <string.h>


/* encode_ntls_to_lpus(const char*, int): uint8_t* {{{1
 *
 * Takes a null-terminated locale string (i.e. char* string) and a length thereof
 * and converts it to a length-prefixed unicode string as is used by Minecraft.
 */
uint8_t *encode_ntls_to_lpus(const char *string, int max_length) {
    int length = (int) strlen(string);
    if (length > max_length) {
        return NULL;
    }

    uint8_t *buffer = calloc(max_length, sizeof(ucs4_t));
    if (!buffer) {
        return NULL;
    }

    unsigned char bytes_written = 0;
    varint_encode(length, (char *) buffer, max_length, &bytes_written);

    memcpy(buffer + bytes_written, string, length);

    return buffer;
}
/* }}}1 */

/* encode_ntus_to_lpus(const char*, int): uint8_t* {{{1
 * 
 * Takes a null-terminated unicode string and a length thereof and converts it to a
 * length-prefixed unicode string as is used by Minecraft.
 */
uint8_t *encode_ntus_to_lpus(const uint8_t *string, int max_length) {
    size_t length = u8_strlen(string);
    if (length > max_length) {
        return NULL;
    }

    uint8_t *buffer = calloc(max_length, sizeof(ucs4_t));
    if (!buffer) {
        return NULL;
    }

    unsigned char bytes_written = 0;
    varint_encode(length, (char *) buffer, max_length, &bytes_written);
    u8_cpy(buffer + bytes_written, string, length);

    return buffer;
}
/* }}}1 */
