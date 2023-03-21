#ifndef UTIL_BYTE_BUFFER_H
#define UTIL_BYTE_BUFFER_H 1

#include "unicode_string.h"
#include "varint.h"

#include <stdint.h>

#define INITIAL_BYTE_BUFFER_SIZE (2 << 12)  /* 8kB start size */
#define MAXIMUM_BYTE_BUFFER_SIZE (2 << 20)  /* 2MB maximum size */
 
extern const int BYTE_BUFFER_INIT_SUCCESS; //0
extern const int BYTE_BUFFER_INIT_FAILURE; //1
extern const int BYTE_BUFFER_GROW_SUCCESS; //0
extern const int BYTE_BUFFER_GROW_TOO_BIG; //1
extern const int BYTE_BUFFER_GROW_REALLOC_FAILURE; //2
extern const int BYTE_BUFFER_READ_SUCCESS; //0
extern const int BYTE_BUFFER_READ_OUT_OF_BOUNDS; //1
extern const int BYTE_BUFFER_READ_FAILURE; //2
extern const int BYTE_BUFFER_WRITE_SUCCESS; //0
extern const int BYTE_BUFFER_WRITE_OUT_OF_BOUNDS; //1
extern const int BYTE_BUFFER_WRITE_FAILURE; //2
extern const int BYTE_BUFFER_WRITE_NETWORK_CLIENT_DISCONNECT; //3

typedef struct byte_buffer_t *byte_buffer_ptr;

typedef struct byte_buffer_t {
    /* the data */
    char *bytes;

    /* bookkeeping */
    int buffer_capacity;
    int buffer_size;
    int read_offset;
    int write_offset;
    int last_read_size;

    /* functions */
    /* helpers */
    char *(*next_bytes)(byte_buffer_ptr self);

    char *(*bytes_at)(byte_buffer_ptr self, int offset);

    int (*remaining_capacity)(byte_buffer_ptr self);

    int (*remaining_length)(byte_buffer_ptr self);

    void (*rewind_last)(byte_buffer_ptr self);

    void (*rewind)(byte_buffer_ptr self, int n_bytes);

    int (*grow)(byte_buffer_ptr self);

    void (*reset)(byte_buffer_ptr self);

    void (*reroll)(byte_buffer_ptr self);

    /* reading */
    int (*read)(byte_buffer_ptr self, int n_bytes, char *out);

    int (*read_bool)(byte_buffer_ptr self, unsigned char *out);

    int (*read_byte)(byte_buffer_ptr self, int8_t *out);

    int (*read_ubyte)(byte_buffer_ptr self, uint8_t *out);

    int (*read_short)(byte_buffer_ptr self, int16_t *out);

    int (*read_ushort)(byte_buffer_ptr self, uint16_t *out);

    int (*read_int)(byte_buffer_ptr self, int32_t *out);

    int (*read_long)(byte_buffer_ptr self, int64_t *out);

    int (*read_float)(byte_buffer_ptr self, float *out);

    int (*read_double)(byte_buffer_ptr self, double *out);

    int (*read_string)(byte_buffer_ptr self, int max_length, UnicodeString *out);

    int (*read_chat)(byte_buffer_ptr self, UnicodeString *out);

    int (*read_identifier)(byte_buffer_ptr self, UnicodeString *out);

    int (*read_varint)(byte_buffer_ptr self, VarInt *out);

    int (*read_varlong)(byte_buffer_ptr self, VarInt *out);

    /* writing */
    int (*write)(byte_buffer_ptr self, char *bytes, int n_bytes);

    int (*write_bool)(byte_buffer_ptr self, unsigned char in);

    int (*write_byte)(byte_buffer_ptr self, int8_t in);

    int (*write_ubyte)(byte_buffer_ptr self, uint8_t in);

    int (*write_short)(byte_buffer_ptr self, int16_t in);

    int (*write_ushort)(byte_buffer_ptr self, uint16_t in);

    int (*write_int)(byte_buffer_ptr self, int32_t in);

    int (*write_long)(byte_buffer_ptr self, int64_t in);

    int (*write_float)(byte_buffer_ptr self, float in);

    int (*write_double)(byte_buffer_ptr self, double in);

    int (*write_nt_string)(byte_buffer_ptr self, int max_length, UnicodeString in);

    int (*write_lp_string)(byte_buffer_ptr self, int length, int max_length, UnicodeString in);

    int (*write_nt_chat)(byte_buffer_ptr self, UnicodeString in);

    int (*write_lp_chat)(byte_buffer_ptr self, int length, UnicodeString in);

    int (*write_nt_identifier)(byte_buffer_ptr self, UnicodeString in);

    int (*write_lp_identifier)(byte_buffer_ptr self, int length, UnicodeString in);

    int (*write_varint)(byte_buffer_ptr self, VarInt in);

    int (*write_varlong)(byte_buffer_ptr self, VarInt in);

    int (*write_network)(byte_buffer_ptr self, int remote_fd);
} byte_buffer_t;

typedef union int8_to_int16 {
    int8_t chars[2];
    int16_t short_value;
} int8_to_int16;

typedef union uint8_to_uint16 {
    uint8_t chars[2];
    uint16_t short_value;
} uint8_to_uint16;

typedef union int8_to_int32 {
    int8_t chars[4];
    int32_t int_value;
} int8_to_int32;

typedef union uint8_to_uint32 {
    uint8_t chars[4];
    uint32_t int_value;
} uint8_to_uint32;

typedef union int8_to_int64 {
    int8_t chars[8];
    int64_t long_value;
} int8_to_int64;

typedef union uint8_to_uint64 {
    uint8_t chars[8];
    uint64_t long_value;
} uint8_to_uint64;

typedef union int8_to_float {
    int8_t chars[4];
    int32_t int_value;
    float float_value;
} int8_to_float;

typedef union int8_to_double {
    int8_t chars[8];
    int64_t long_value;
    double double_value;
} int8_to_double;

int init_byte_buffer(byte_buffer_t *buf);

void free_byte_buffer(byte_buffer_t *buf);

#endif
