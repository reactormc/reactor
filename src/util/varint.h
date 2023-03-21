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

#ifndef UTIL_VARINT_H
#define UTIL_VARINT_H 1

#define VARINT_MAX_LEN 5
#define VARLONG_MAX_LEN 10

typedef unsigned long long VarInt; // Convenience
char *varint_encode(VarInt varint, char *buf, int buf_len, unsigned char *bytes_written);

VarInt varint_decode(char *buf, int buf_len, unsigned char *bytes_read);

VarInt varint_decode_offset(char *buffer, int length, int *offset); // Convenience
int varint_encoding_length(VarInt);

#endif
