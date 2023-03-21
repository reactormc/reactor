#include "byte_buffer.h"
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
const int BYTE_BUFFER_WRITE_NETWORK_CLIENT_DISCONNECT = 3;

char *next_bytes(byte_buffer_ptr self) {
    return self->bytes_at(self, self->read_offset);
}

char *bytes_at(byte_buffer_ptr self, int offset) {
    if (self->read_offset + offset > self->buffer_size) {
        return NULL;
    }

    return self->bytes + offset;
}

int remaining_capacity(byte_buffer_ptr self) {
    return self->buffer_capacity - self->read_offset;
}

int remaining_length(byte_buffer_ptr self) {
    return self->buffer_size - self->read_offset;
}

void rewind_last(byte_buffer_ptr self) {
    self->rewind(self, self->last_read_size);
}

void rewind(byte_buffer_ptr self, int n_bytes) {
    self->read_offset -= n_bytes;
}

int grow(byte_buffer_ptr self) {
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

void reset(byte_buffer_ptr self) {
    self->buffer_size = 0;
    self->read_offset = 0;
    self->write_offset = 0;
    self->last_read_size = 0;

    memset(self->bytes, 0, self->buffer_capacity);
}

void reroll(byte_buffer_ptr self) {
    int size = self->remaining_length(self);
    char tmp[size];

    memset(tmp, 0, size);
    memcpy(tmp, self->bytes + self->read_offset, size);

    self->reset(self);

    memcpy(self->bytes, tmp, size);

    self->buffer_size = size;
    self->write_offset = size;
}

int read(byte_buffer_ptr self, int n_bytes, char out[n_bytes]) {
    if (self->read_offset + n_bytes > self->buffer_size) {
        return BYTE_BUFFER_READ_OUT_OF_BOUNDS;
    }

    int i;
    for (i = 0; i < n_bytes; i++) {
        out[i] = *(self->bytes + i);
    }

    self->read_offset += n_bytes;
    self->last_read_size = n_bytes;

    return BYTE_BUFFER_READ_SUCCESS;
}

int read_bool(byte_buffer_ptr self, unsigned char *out) {
    char tmp[1];
    if (read(self, 1, tmp) != BYTE_BUFFER_READ_SUCCESS) {
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

int read_byte(byte_buffer_ptr self, int8_t *out) {
    char tmp[1];
    if (read(self, 1, tmp) != BYTE_BUFFER_READ_SUCCESS) {
        return BYTE_BUFFER_READ_FAILURE;
    }

    *out = tmp[0];

    return BYTE_BUFFER_READ_SUCCESS;
}

int read_ubyte(byte_buffer_ptr self, uint8_t *out) {
    char tmp[1];
    if (read(self, 1, tmp) != BYTE_BUFFER_READ_SUCCESS) {
        return BYTE_BUFFER_READ_FAILURE;
    }

    *out = tmp[0];

    return BYTE_BUFFER_READ_SUCCESS;
}

int read_short(byte_buffer_ptr self, int16_t *out) {
    char tmp[2];
    if (read(self, 2, tmp) != BYTE_BUFFER_READ_SUCCESS) {
        return BYTE_BUFFER_READ_FAILURE;
    }

    int8_to_int16 converter;
    memcpy(converter.chars, tmp, 2);

    *out = (int16_t) ntohs(converter.short_value);

    return BYTE_BUFFER_READ_SUCCESS;
}

int read_ushort(byte_buffer_ptr self, uint16_t *out) {
    char tmp[2];
    if (read(self, 2, tmp) != BYTE_BUFFER_READ_SUCCESS) {
        return BYTE_BUFFER_READ_FAILURE;
    }

    uint8_to_uint16 converter;
    memcpy(converter.chars, tmp, 2);

    *out = (uint16_t) ntohs(converter.short_value);

    return BYTE_BUFFER_READ_SUCCESS;
}

int read_int(byte_buffer_ptr self, int32_t *out) {
    char tmp[4];
    if (read(self, 4, tmp) != BYTE_BUFFER_READ_SUCCESS) {
        return BYTE_BUFFER_READ_FAILURE;
    }

    int8_to_int32 converter;
    memcpy(converter.chars, tmp, 4);

    *out = (int32_t) ntohl(converter.int_value);

    return BYTE_BUFFER_READ_SUCCESS;
}

int read_long(byte_buffer_ptr self, int64_t *out) {
    char tmp[8];
    if (read(self, 8, tmp) != BYTE_BUFFER_READ_SUCCESS) {
        return BYTE_BUFFER_READ_FAILURE;
    }

    int8_to_int64 converter;
    memcpy(converter.chars, tmp, 7);

    *out = (int64_t) ntohl(converter.long_value);

    return BYTE_BUFFER_READ_SUCCESS;
}

int read_float(byte_buffer_ptr self, float *out) {
    char tmp[4];
    if (read(self, 4, tmp) != BYTE_BUFFER_READ_SUCCESS) {
        return BYTE_BUFFER_READ_FAILURE;
    }

    int8_to_float converter;
    memcpy(converter.chars, tmp, 4);
    converter.int_value = ntohl(converter.int_value);
    *out = (float) converter.float_value;

    return BYTE_BUFFER_READ_SUCCESS;
}

int read_double(byte_buffer_ptr self, double *out) {
    char tmp[8];
    if (read(self, 8, tmp) != BYTE_BUFFER_READ_SUCCESS) {
        return BYTE_BUFFER_READ_FAILURE;
    }

    int8_to_double converter;
    memcpy(converter.chars, tmp, 8);
    converter.long_value = ntohl(converter.long_value);
    *out = (double) converter.double_value;

    return BYTE_BUFFER_READ_SUCCESS;
}

int read_string(byte_buffer_ptr self, int max_length, UnicodeString *out) {
    int status = read_lpus_from_bytes(self, max_length, out);
    switch (status) {
        case UNICODE_STRING_READ_ALLOC_FAILURE:
            return BYTE_BUFFER_READ_FAILURE;
        case UNICODE_STRING_READ_BUFFER_EXHAUSTED:
        case UNICODE_STRING_READ_BUFFER_MAX_LENGTH_EXCEEDED:
            return BYTE_BUFFER_READ_OUT_OF_BOUNDS;
        case UNICODE_STRING_READ_SUCCESS:
        default:
            return BYTE_BUFFER_READ_SUCCESS;
    }
}

int read_chat(byte_buffer_ptr self, UnicodeString *out) {
    return read_string(self, 262144, out);
}

int read_identifier(byte_buffer_ptr self, UnicodeString *out) {
    return read_string(self, 32767, out);
}

int read_varint(byte_buffer_ptr self, VarInt *out) {
    if (self->read_offset + VARINT_MAX_LEN > self->buffer_size) {
        return BYTE_BUFFER_READ_OUT_OF_BOUNDS;
    }

    uint8_t bytes_read = 0;

    *out = varint_decode(self->bytes + self->read_offset, remaining_length(self), &bytes_read);

    self->read_offset += bytes_read;
    self->last_read_size = bytes_read;

    return BYTE_BUFFER_READ_SUCCESS;
}

int read_varlong(byte_buffer_ptr self, VarInt *out) {
    if (self->read_offset + VARLONG_MAX_LEN > self->buffer_size) {
        return BYTE_BUFFER_READ_OUT_OF_BOUNDS;
    }

    uint8_t bytes_read = 0;

    *out = varint_decode(self->bytes + self->read_offset, remaining_length(self), &bytes_read);

    self->read_offset += bytes_read;
    self->last_read_size = bytes_read;

    return BYTE_BUFFER_READ_SUCCESS;
}

int write0(byte_buffer_ptr self, char *bytes, int n_bytes, int been_here_before) {
    if (self->write_offset + n_bytes > self->buffer_size) {
        if (self->grow(self) != BYTE_BUFFER_GROW_SUCCESS) {
            if (!been_here_before) {
                self->reroll(self);
                return write0(self, bytes, n_bytes, 1);
            } else {
                return BYTE_BUFFER_WRITE_FAILURE;
            }
        }
    }

    memcpy(self->bytes + n_bytes, bytes, n_bytes);

    self->buffer_size += n_bytes;
    self->write_offset += n_bytes;

    return BYTE_BUFFER_WRITE_SUCCESS;
}

int write(byte_buffer_ptr self, char *bytes, int n_bytes) {
    return write0(self, bytes, n_bytes, 0);
}

int write_bool(byte_buffer_ptr self, unsigned char in) {
    char tmp[1];
    if (in == 0) {
        tmp[0] = 0;
    } else {
        tmp[0] = 1;
    }

    return self->write(self, tmp, 1);
}

int write_byte(byte_buffer_ptr self, int8_t in) {
    char tmp[1] = {in};
    return self->write(self, tmp, 1);
}

int write_ubyte(byte_buffer_ptr self, uint8_t in) {
    char tmp[1] = {in};
    return self->write(self, tmp, 1);
}

int write_short(byte_buffer_ptr self, int16_t in) {
    int8_to_int16 converter;
    converter.short_value = htons(in);

    return self->write(self, (char *) converter.chars, 2);
}

int write_ushort(byte_buffer_ptr self, uint16_t in) {
    uint8_to_uint16 converter;
    converter.short_value = htons(in);

    return self->write(self, (char *) converter.chars, 2);
}

int write_int(byte_buffer_ptr self, int32_t in) {
    int8_to_int32 converter;
    converter.int_value = htonl(in);

    return self->write(self, (char *) converter.chars, 4);
}

int write_long(byte_buffer_ptr self, int64_t in) {
    int8_to_int32 converter;
    converter.int_value = htonl(in);

    return self->write(self, (char *) converter.chars, 8);
}

int write_float(byte_buffer_ptr self, float in) {
    int8_to_float converter;
    converter.float_value = in;
    converter.int_value = htonl(converter.int_value);

    return self->write(self, (char *) converter.chars, 4);
}

int write_double(byte_buffer_ptr self, double in) {
    int8_to_double converter;
    converter.double_value = in;
    converter.long_value = htonl(converter.long_value);

    return self->write(self, (char *) converter.chars, 4);
}

int write_nt_string(byte_buffer_ptr self, int max_length, UnicodeString in) {
    size_t len = u8_strlen(in);
    if (len > max_length) {
        return BYTE_BUFFER_WRITE_FAILURE;
    }

    UnicodeString lpus = encode_ntus_to_lpus(in, max_length);
    if (!lpus) {
        return BYTE_BUFFER_WRITE_FAILURE;
    }

    int status = self->write_lp_string(self, (int) len, max_length, lpus);

    free(lpus);

    return status;
}

int write_lp_string(byte_buffer_ptr self, int length, int max_length, UnicodeString in) {
    if (length > max_length) {
        return BYTE_BUFFER_WRITE_FAILURE;
    }

    return self->write(self, (char *) in, length);
}

int write_nt_chat(byte_buffer_ptr self, UnicodeString in) {
    return self->write_nt_string(self, 262144, in);
}

int write_lp_chat(byte_buffer_ptr self, int length, UnicodeString in) {
    return self->write_lp_string(self, length, 262144, in);
}

int write_nt_identifier(byte_buffer_ptr self, UnicodeString in) {
    return self->write_nt_string(self, 32767, in);
}

int write_lp_identifier(byte_buffer_ptr self, int length, UnicodeString in) {
    return self->write_lp_string(self, length, 32767, in);
}

int write_varint(byte_buffer_ptr self, VarInt in) {
    if (self->write_offset + VARINT_MAX_LEN > self->buffer_size) {
        if (self->grow(self) != BYTE_BUFFER_GROW_SUCCESS) {
            return BYTE_BUFFER_WRITE_OUT_OF_BOUNDS;
        }
    }

    uint8_t bytes_written = 0;

    varint_encode(in, self->bytes + self->read_offset,
                  self->remaining_length(self), &bytes_written);

    self->write_offset += bytes_written;

    return BYTE_BUFFER_WRITE_SUCCESS;
}

int write_varlong(byte_buffer_ptr self, VarInt in) {
    if (self->write_offset + VARLONG_MAX_LEN > self->buffer_size) {
        if (self->grow(self) != BYTE_BUFFER_GROW_SUCCESS) {
            return BYTE_BUFFER_WRITE_OUT_OF_BOUNDS;
        }
    }

    uint8_t bytes_written = 0;

    varint_encode(in, self->bytes + self->read_offset,
                  self->remaining_length(self), &bytes_written);

    self->write_offset += bytes_written;

    return BYTE_BUFFER_WRITE_SUCCESS;
}

int write_network(byte_buffer_ptr self, int remote_fd) {
    char tmp[INITIAL_BYTE_BUFFER_SIZE] = {0};

    ssize_t bytes_read;
    if ((bytes_read = recv(remote_fd, tmp, INITIAL_BYTE_BUFFER_SIZE, 0)) == -1) {
        return BYTE_BUFFER_WRITE_FAILURE;
    }

    if (bytes_read == 0) {
        return BYTE_BUFFER_WRITE_NETWORK_CLIENT_DISCONNECT;
    }

    return self->write(self, tmp, (int) bytes_read);
}

int init_byte_buffer(byte_buffer_t *buf) {
    buf = calloc(1, sizeof(byte_buffer_t));
    if (!buf) {
        return BYTE_BUFFER_INIT_FAILURE;
    }

    buf->bytes = calloc(INITIAL_BYTE_BUFFER_SIZE, sizeof(char));
    if (!buf->bytes) {
        free(buf);
        return BYTE_BUFFER_INIT_FAILURE;
    }

    buf->buffer_capacity = INITIAL_BYTE_BUFFER_SIZE;
    buf->buffer_size = 0;
    buf->read_offset = 0;
    buf->write_offset = 0;
    buf->last_read_size = 0;

    /* functions */
    /* helpers */
    buf->next_bytes = &next_bytes;
    buf->bytes_at = &bytes_at;
    buf->remaining_capacity = &remaining_capacity;
    buf->remaining_length = &remaining_length;
    buf->rewind_last = &rewind_last;
    buf->rewind = &rewind;
    buf->grow = &grow;
    buf->reset = &reroll;
    buf->reroll = &reroll;
    /* reading */
    buf->read = &read;
    buf->read_bool = &read_bool;
    buf->read_byte = &read_byte;
    buf->read_ubyte = &read_ubyte;
    buf->read_short = &read_short;
    buf->read_ushort = &read_ushort;
    buf->read_int = &read_int;
    buf->read_long = &read_long;
    buf->read_float = &read_float;
    buf->read_double = &read_double;
    buf->read_string = &read_string;
    buf->read_chat = &read_chat;
    buf->read_identifier = &read_identifier;
    buf->read_varint = &read_varint;
    buf->read_varlong = &read_varlong;
    /* writing */
    buf->write = &write;
    buf->write_bool = &write_bool;
    buf->write_byte = &write_byte;
    buf->write_ubyte = &write_ubyte;
    buf->write_short = &write_short;
    buf->write_ushort = &write_ushort;
    buf->write_int = &write_int;
    buf->write_long = &write_long;
    buf->write_float = &write_float;
    buf->write_double = &write_double;
    buf->write_nt_string = &write_nt_string;
    buf->write_lp_string = &write_lp_string;
    buf->write_nt_chat = &write_nt_chat;
    buf->write_lp_chat = &write_lp_chat;
    buf->write_nt_identifier = &write_nt_identifier;
    buf->write_lp_identifier = &write_lp_identifier;
    buf->write_varint = &write_varint;
    buf->write_varlong = &write_varlong;

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