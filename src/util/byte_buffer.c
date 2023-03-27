#include "byte_buffer.h"
#include "logger.h"
#include "unicode_string.h"

#include <string.h>
#include <arpa/inet.h>

const int BYTE_BUFFER_INIT_SUCCESS = 0;
const int BYTE_BUFFER_INIT_FAILURE = 1;
const int BYTE_BUFFER_GROW_SUCCESS = 0;
const int BYTE_BUFFER_GROW_TOO_BIG = 1;
const int BYTE_BUFFER_GROW_REALLOC_FAILURE = 2;
const int BYTE_BUFFER_READ_SUCCESS = 0;
const int BYTE_BUFFER_READ_OUT_OF_BOUNDS = 1;
const int BYTE_BUFFER_READ_FAILURE = 2;
const int BYTE_BUFFER_WRITE_SUCCESS = 0;
const int BYTE_BUFFER_WRITE_OUT_OF_BOUNDS = 1;
const int BYTE_BUFFER_WRITE_FAILURE = 2;

static char *bb_next_bytes(byte_buffer_ptr self) {
    return self->bytes_at(self, self->read_offset);
}

static char *bb_bytes_at(byte_buffer_ptr self, int offset) {
    if (offset > self->buffer_size) {
        return NULL;
    }

    return self->bytes + offset;
}

static int bb_remaining_capacity(byte_buffer_ptr self) {
    return self->buffer_capacity - self->read_offset;
}

static int bb_remaining_length(byte_buffer_ptr self) {
    return self->buffer_size - self->read_offset;
}

static void bb_roll_back_last(byte_buffer_ptr self) {
    self->roll_back(self, self->last_read_size);
}

static void bb_roll_back(byte_buffer_ptr self, int n_bytes) {
    self->read_offset -= n_bytes;
}

static int bb_grow(byte_buffer_ptr self) {
    int new_capacity = self->buffer_capacity * 2;
    if (new_capacity > MAXIMUM_BYTE_BUFFER_SIZE) {
        return BYTE_BUFFER_GROW_TOO_BIG;
    }

    char *tmp = reallocarray(self->bytes, sizeof(char), new_capacity);
    if (!tmp) {
        return BYTE_BUFFER_GROW_REALLOC_FAILURE;
    }

    self->bytes = tmp;
    self->buffer_capacity = new_capacity;

    return BYTE_BUFFER_GROW_SUCCESS;
}

static void bb_reset(byte_buffer_ptr self) {
    self->buffer_size = 0;
    self->read_offset = 0;
    self->write_offset = 0;
    self->last_read_size = 0;

    memset(self->bytes, 0, self->buffer_capacity);
}

static void bb_reroll(byte_buffer_ptr self) {
    int size = self->remaining_length(self);
    char tmp[size];

    memset(tmp, 0, size);
    memcpy(tmp, self->bytes + self->read_offset, size);

    self->reset(self);

    memcpy(self->bytes, tmp, size);

    self->buffer_size = size;
    self->write_offset = size;
}

static int bb_is_empty(byte_buffer_ptr self) {
    return self->buffer_size == 0 ? 1 : 0;
}

static int bb_read(byte_buffer_ptr self, int n_bytes, char out[n_bytes]) {
    if (self->read_offset + n_bytes > self->buffer_size) {
        return BYTE_BUFFER_READ_OUT_OF_BOUNDS;
    }

    int i;
    for (i = 0; i < n_bytes; i++) {
        out[i] = *(self->next_bytes(self) + i);
    }

    self->read_offset += n_bytes;
    self->last_read_size = n_bytes;

    return BYTE_BUFFER_READ_SUCCESS;
}

static int bb_read_bool(byte_buffer_ptr self, unsigned char *out) {
    char tmp[1];
    if (self->read(self, 1, tmp) != BYTE_BUFFER_READ_SUCCESS) {
        return BYTE_BUFFER_READ_FAILURE;
    }

    // 0 is false, anything else is truthy.
    if (tmp[0] == 0) {
        *out = 0;
    } else {
        *out = 1;
    }

    return BYTE_BUFFER_READ_SUCCESS;
}

static int bb_read_byte(byte_buffer_ptr self, int8_t *out) {
    char tmp[1];
    if (self->read(self, 1, tmp) != BYTE_BUFFER_READ_SUCCESS) {
        return BYTE_BUFFER_READ_FAILURE;
    }

    *out = tmp[0];

    return BYTE_BUFFER_READ_SUCCESS;
}

static int bb_read_ubyte(byte_buffer_ptr self, uint8_t *out) {
    char tmp[1];
    if (self->read(self, 1, tmp) != BYTE_BUFFER_READ_SUCCESS) {
        return BYTE_BUFFER_READ_FAILURE;
    }

    *out = tmp[0];

    return BYTE_BUFFER_READ_SUCCESS;
}

static int bb_read_short(byte_buffer_ptr self, int16_t *out) {
    char tmp[2];
    if (self->read(self, 2, tmp) != BYTE_BUFFER_READ_SUCCESS) {
        return BYTE_BUFFER_READ_FAILURE;
    }

    int8_to_int16 converter;
    memcpy(converter.chars, tmp, 2);

    *out = (int16_t) ntohs(converter.short_value);

    return BYTE_BUFFER_READ_SUCCESS;
}

static int bb_read_ushort(byte_buffer_ptr self, uint16_t *out) {
    char tmp[2];
    if (self->read(self, 2, tmp) != BYTE_BUFFER_READ_SUCCESS) {
        return BYTE_BUFFER_READ_FAILURE;
    }

    uint8_to_uint16 converter;
    memcpy(converter.chars, tmp, 2);

    *out = (uint16_t) ntohs(converter.short_value);

    return BYTE_BUFFER_READ_SUCCESS;
}

static int bb_read_int(byte_buffer_ptr self, int32_t *out) {
    char tmp[4];
    if (self->read(self, 4, tmp) != BYTE_BUFFER_READ_SUCCESS) {
        return BYTE_BUFFER_READ_FAILURE;
    }

    int8_to_int32 converter;
    memcpy(converter.chars, tmp, 4);

    *out = (int32_t) ntohl(converter.int_value);

    return BYTE_BUFFER_READ_SUCCESS;
}

static int bb_read_long(byte_buffer_ptr self, int64_t *out) {
    char tmp[8];
    if (self->read(self, 8, tmp) != BYTE_BUFFER_READ_SUCCESS) {
        return BYTE_BUFFER_READ_FAILURE;
    }

    int8_to_int64 converter;
    memcpy(converter.chars, tmp, 7);

    *out = (int64_t) ntohl(converter.long_value);

    return BYTE_BUFFER_READ_SUCCESS;
}

static int bb_read_float(byte_buffer_ptr self, float *out) {
    char tmp[4];
    if (self->read(self, 4, tmp) != BYTE_BUFFER_READ_SUCCESS) {
        return BYTE_BUFFER_READ_FAILURE;
    }

    int8_to_float converter;
    memcpy(converter.chars, tmp, 4);
    converter.int_value = ntohl(converter.int_value);
    *out = (float) converter.float_value;

    return BYTE_BUFFER_READ_SUCCESS;
}

static int bb_read_double(byte_buffer_ptr self, double *out) {
    char tmp[8];
    if (self->read(self, 8, tmp) != BYTE_BUFFER_READ_SUCCESS) {
        return BYTE_BUFFER_READ_FAILURE;
    }

    int8_to_double converter;
    memcpy(converter.chars, tmp, 8);
    converter.long_value = ntohl(converter.long_value);
    *out = (double) converter.double_value;

    return BYTE_BUFFER_READ_SUCCESS;
}

static int bb_read_string(byte_buffer_ptr self, int max_length, uint8_t **out) {
    int buffer_size = 0, buffer_len = INITIAL_UNICODE_STRING_SIZE;

    *out = calloc(buffer_len, sizeof(ucs4_t));
    if (!*out) {
        return BYTE_BUFFER_READ_FAILURE;
    }

    VarInt str_len = 0;
    if (self->read_varint(self, &str_len) == BYTE_BUFFER_READ_OUT_OF_BOUNDS) {
        free(*out);
        *out = NULL;
        return BYTE_BUFFER_READ_OUT_OF_BOUNDS;
    }

    if (str_len > self->remaining_length(self)) {
        free(*out);
        *out = NULL;
        self->roll_back_last(self);
        return BYTE_BUFFER_READ_OUT_OF_BOUNDS;
    }

    if (str_len > max_length) {
        free(*out);
        *out = NULL;
        self->roll_back_last(self);
        return BYTE_BUFFER_READ_OUT_OF_BOUNDS;
    }

    int i, total_read = self->last_read_size;
    for (i = 0; i < str_len; i++) {
        ucs4_t codepoint = (ucs4_t) *self->next_bytes(self);

        buffer_size++;
        if (buffer_size + 1 > buffer_len) {
            buffer_len += UNICODE_STRING_SIZE_INCR;

            uint8_t *tmp = realloc(out, buffer_len);
            if (!tmp) {
                free(out);
                *out = NULL;
                self->roll_back(self, total_read);
                return BYTE_BUFFER_READ_FAILURE;
            }

            *out = tmp;
        }

        *(*out + i) = codepoint;
        *(*out + i + 1) = '\0';

        self->read_offset++;

        total_read++;
    }

    self->last_read_size = total_read;
    return BYTE_BUFFER_READ_SUCCESS;
}

static int bb_read_chat(byte_buffer_ptr self, uint8_t **out) {
    return self->read_string(self, 262144, out);
}

static int bb_read_identifier(byte_buffer_ptr self, uint8_t **out) {
    return self->read_string(self, 32767, out);
}

static int bb_read_varint(byte_buffer_ptr self, VarInt *out) {
    if (self->read_offset + 1 > self->buffer_size) {
        debug("bb_read_varint: cannot read varint, out of bounds\n");
        return BYTE_BUFFER_READ_OUT_OF_BOUNDS;
    }

    uint8_t bytes_read = 0;

    *out = varint_decode(self->next_bytes(self), self->remaining_length(self), &bytes_read);

    debug("bb_read_varint: read varint %llu\n", *out);

    self->read_offset += bytes_read;
    self->last_read_size = bytes_read;

    return BYTE_BUFFER_READ_SUCCESS;
}

static int bb_write0(byte_buffer_ptr self, char *bytes, int n_bytes, int been_here_before) {
    if (self->write_offset + n_bytes > self->buffer_capacity) {
        if (self->grow(self) != BYTE_BUFFER_GROW_SUCCESS) {
            if (!been_here_before) {
                self->reroll(self);
                return bb_write0(self, bytes, n_bytes, 1);
            } else {
                return BYTE_BUFFER_WRITE_FAILURE;
            }
        }
    }

    memcpy(self->bytes + self->write_offset, bytes, n_bytes);

    self->buffer_size += n_bytes;
    self->write_offset += n_bytes;

    return BYTE_BUFFER_WRITE_SUCCESS;
}

static int bb_write(byte_buffer_ptr self, char *bytes, int n_bytes) {
    return bb_write0(self, bytes, n_bytes, 0);
}

static int bb_write_bool(byte_buffer_ptr self, unsigned char in) {
    char tmp[1];
    if (in == 0) {
        tmp[0] = 0;
    } else {
        tmp[0] = 1;
    }

    return self->write(self, tmp, 1);
}

static int bb_write_byte(byte_buffer_ptr self, int8_t in) {
    char tmp[1] = {in};
    return self->write(self, tmp, 1);
}

static int bb_write_ubyte(byte_buffer_ptr self, uint8_t in) {
    char tmp[1] = {in};
    return self->write(self, tmp, 1);
}

static int bb_write_short(byte_buffer_ptr self, int16_t in) {
    int8_to_int16 converter;
    converter.short_value = htons(in);

    return self->write(self, (char *) converter.chars, 2);
}

static int bb_write_ushort(byte_buffer_ptr self, uint16_t in) {
    uint8_to_uint16 converter;
    converter.short_value = htons(in);

    return self->write(self, (char *) converter.chars, 2);
}

static int bb_write_int(byte_buffer_ptr self, int32_t in) {
    int8_to_int32 converter;
    converter.int_value = htonl(in);

    return self->write(self, (char *) converter.chars, 4);
}

static int bb_write_long(byte_buffer_ptr self, int64_t in) {
    int8_to_int32 converter;
    converter.int_value = htonl(in);

    return self->write(self, (char *) converter.chars, 8);
}

static int bb_write_float(byte_buffer_ptr self, float in) {
    int8_to_float converter;
    converter.float_value = in;
    converter.int_value = htonl(converter.int_value);

    return self->write(self, (char *) converter.chars, 4);
}

static int bb_write_double(byte_buffer_ptr self, double in) {
    int8_to_double converter;
    converter.double_value = in;
    converter.long_value = htonl(converter.long_value);

    return self->write(self, (char *) converter.chars, 4);
}

static int bb_write_nt_string(byte_buffer_ptr self, int max_length, uint8_t *in) {
    size_t len = u8_strlen(in);
    if (len > max_length) {
        return BYTE_BUFFER_WRITE_FAILURE;
    }

    uint8_t *lpus = encode_ntus_to_lpus(in, max_length);
    if (!lpus) {
        return BYTE_BUFFER_WRITE_FAILURE;
    }

    int status = self->write_lp_string(self, (int) len, max_length, lpus);

    free(lpus);

    return status;
}

static int bb_write_lp_string(byte_buffer_ptr self, int length, int max_length, uint8_t *in) {
    if (length > max_length) {
        return BYTE_BUFFER_WRITE_FAILURE;
    }

    return self->write(self, (char *) in, length);
}

static int bb_write_nt_chat(byte_buffer_ptr self, uint8_t *in) {
    return self->write_nt_string(self, 262144, in);
}

static int bb_write_lp_chat(byte_buffer_ptr self, int length, uint8_t *in) {
    return self->write_lp_string(self, length, 262144, in);
}

static int bb_write_nt_identifier(byte_buffer_ptr self, uint8_t *in) {
    return self->write_nt_string(self, 32767, in);
}

static int bb_write_lp_identifier(byte_buffer_ptr self, int length, uint8_t *in) {
    return self->write_lp_string(self, length, 32767, in);
}

static int bb_write_varint(byte_buffer_ptr self, VarInt in) {
    if (self->write_offset + 1 > self->buffer_size) {
        if (self->grow(self) != BYTE_BUFFER_GROW_SUCCESS) {
            return BYTE_BUFFER_WRITE_OUT_OF_BOUNDS;
        }
    }

    uint8_t bytes_written = 0;

    varint_encode(in, self->bytes + self->write_offset,
                  self->remaining_length(self), &bytes_written);

    self->write_offset += bytes_written;

    return BYTE_BUFFER_WRITE_SUCCESS;
}

int init_byte_buffer(byte_buffer_t **buf) {
    *buf = calloc(1, sizeof(byte_buffer_t));
    if (!*buf) {
        return BYTE_BUFFER_INIT_FAILURE;
    }

    (*buf)->bytes = calloc(INITIAL_BYTE_BUFFER_SIZE, sizeof(char));
    if (!(*buf)->bytes) {
        free(buf);
        return BYTE_BUFFER_INIT_FAILURE;
    }

    (*buf)->buffer_capacity = INITIAL_BYTE_BUFFER_SIZE;
    (*buf)->buffer_size = 0;
    (*buf)->read_offset = 0;
    (*buf)->write_offset = 0;
    (*buf)->last_read_size = 0;

    /* functions */

    /* helpers */
    (*buf)->next_bytes = &bb_next_bytes;
    (*buf)->bytes_at = &bb_bytes_at;
    (*buf)->remaining_capacity = &bb_remaining_capacity;
    (*buf)->remaining_length = &bb_remaining_length;
    (*buf)->roll_back_last = &bb_roll_back_last;
    (*buf)->roll_back = &bb_roll_back;
    (*buf)->grow = &bb_grow;
    (*buf)->reset = &bb_reset;
    (*buf)->reroll = &bb_reroll;
    (*buf)->is_empty = &bb_is_empty;

    /* reading */
    (*buf)->read = &bb_read;
    (*buf)->read_bool = &bb_read_bool;
    (*buf)->read_byte = &bb_read_byte;
    (*buf)->read_ubyte = &bb_read_ubyte;
    (*buf)->read_short = &bb_read_short;
    (*buf)->read_ushort = &bb_read_ushort;
    (*buf)->read_int = &bb_read_int;
    (*buf)->read_long = &bb_read_long;
    (*buf)->read_float = &bb_read_float;
    (*buf)->read_double = &bb_read_double;
    (*buf)->read_string = &bb_read_string;
    (*buf)->read_chat = &bb_read_chat;
    (*buf)->read_identifier = &bb_read_identifier;
    (*buf)->read_varint = &bb_read_varint;

    /* writing */
    (*buf)->write = &bb_write;
    (*buf)->write_bool = &bb_write_bool;
    (*buf)->write_byte = &bb_write_byte;
    (*buf)->write_ubyte = &bb_write_ubyte;
    (*buf)->write_short = &bb_write_short;
    (*buf)->write_ushort = &bb_write_ushort;
    (*buf)->write_int = &bb_write_int;
    (*buf)->write_long = &bb_write_long;
    (*buf)->write_float = &bb_write_float;
    (*buf)->write_double = &bb_write_double;
    (*buf)->write_nt_string = &bb_write_nt_string;
    (*buf)->write_lp_string = &bb_write_lp_string;
    (*buf)->write_nt_chat = &bb_write_nt_chat;
    (*buf)->write_lp_chat = &bb_write_lp_chat;
    (*buf)->write_nt_identifier = &bb_write_nt_identifier;
    (*buf)->write_lp_identifier = &bb_write_lp_identifier;
    (*buf)->write_varint = &bb_write_varint;

    return BYTE_BUFFER_INIT_SUCCESS;
}

void free_byte_buffer(byte_buffer_t *buf) {
    if (!buf) {
        return;
    }

    if (buf->bytes) {
        free(buf->bytes);
    }

    free(buf);
}
