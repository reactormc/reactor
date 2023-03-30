/**
 * Copyright 2014 Eduardo Sorribas
 * http://sorribas.org
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * Note: original source from https://github.com/sorribas/varint.c
 * Above copyright applies.
 *
 * Some changes are made by Jacob Andersen.
 */

#include <assert.h>
#include "varint.h"

#define NULL 0

static const char MSB = 0x80;
static const char MSBALL = ~0x7F;

static const VarInt N1 = 128; // 2 ^ 7
static const VarInt N2 = 16384;
static const VarInt N3 = 2097152;
static const VarInt N4 = 268435456;
static const VarInt N5 = 34359738368;
static const VarInt N6 = 4398046511104;
static const VarInt N7 = 562949953421312;
static const VarInt N8 = 72057594037927936;
static const VarInt N9 = 9223372036854775808U;

char *varint_encode(VarInt n, char *buf, int buf_len, unsigned char *bytes_written) {
    char *ptr = buf;

    while (n & MSBALL) {
        *(ptr++) = (n & 0xFF) | MSB;
        n = n >> 7;
        assert((ptr - buf) < buf_len);
    }
    *ptr = n;
    if (bytes_written != NULL) *bytes_written = ptr - buf + 1;

    return buf;
}

VarInt varint_decode(char *buf, int buf_len, unsigned char *bytes_read) {
    VarInt result = 0;
    int bits = 0;
    char *ptr = buf;
    VarInt ll;
    while (*ptr & MSB) {
        ll = *ptr;
        result += ((ll & 0x7F) << bits);
        ptr++;
        bits += 7;
        assert((ptr - buf) < buf_len);
    }
    ll = *ptr;
    result += ((ll & 0x7F) << bits);

    if (bytes_read != NULL) *bytes_read = ptr - buf + 1;

    return result;
}

VarInt varint_decode_offset(char *buffer, int length, int *offset) {
    unsigned char bytes_read = 0;

    unsigned long long varint = varint_decode(buffer + *offset, length - *offset, &bytes_read);

    *offset += bytes_read;

    return varint;
}

int varint_encoding_length(VarInt n) {
    return (
            n < N1 ? 1
                   : n < N2 ? 2
                            : n < N3 ? 3
                                     : n < N4 ? 4
                                              : n < N5 ? 5
                                                       : n < N6 ? 6
                                                                : n < N7 ? 7
                                                                         : n < N8 ? 8
                                                                                  : n < N9 ? 9
                                                                                           : 10
    );
}
