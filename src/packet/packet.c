#include "packet.h"

#include "util/logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

ReactorPacketPtr create_empty_packet(int packet_id) {
    ReactorPacketPtr ret = calloc(1, sizeof(ReactorPacket));
    if (!ret) {
        perror("create_empty_packet - calloc");
        return NULL;
    }

    ret->packet_length = varint_encoding_length(packet_id);
    ret->packet_id = packet_id;

    return ret;
}

int create_packet_from_header(char *buffer, int length, int *offset, int compressed, ReactorPacketPtr *packet) {
    *packet = calloc(1, sizeof(ReactorPacket));
    if (!*packet) {
        perror("create_packet_from_header: packet calloc");
        return -1;
    }

    if (*offset < 0 || *offset >= length) {
        debug("create_packet_from_header: offset out of bounds\n");
        free(packet);
        return -2;
    }

    int remaining_length = length - *offset;

    if (length - *offset < VARINT_MAX_LEN) {
        debug("Cannot read a full varint\n");
        free(packet);
        return -2;
    }

    int packet_length = varint_decode_offset(buffer, length, offset);

    if (packet_length == 0) {
        debug("create_packet_from_header: done reading packets in buffer\n");
        return 1;
    } else if (packet_length > remaining_length) {
        debug("create_packet_from_header: cannot read entire packet\n");
        return -2;
    }

    if (compressed) {
        (*packet)->data_length = varint_decode_offset(buffer, length, offset);
    }

    int offset_before = *offset;
    (*packet)->packet_id = varint_decode_offset(buffer, length, offset);

    // Packet length updated to be the length of remaining buffer data
    packet_length -= (*offset - offset_before);
    (*packet)->packet_length = packet_length;

    char *data_buf = calloc(packet_length, sizeof(char));
    if (!data_buf) {
        perror("create_packet_from_header: data_buf calloc");
        return -1;
    }

    strncpy(data_buf, buffer + *offset, packet_length);
    (*packet)->data = data_buf;
    *offset += packet_length;

    return 0;
}

char *encode_packet(ReactorPacketPtr packet) {
    int buffer_len = packet->packet_length + varint_encoding_length(packet->packet_length);

    char *buffer = calloc(buffer_len, sizeof(char));
    if (!buffer) {
        perror("encode_packet - calloc");
        return NULL;
    }

    unsigned char bytes_tmp = 0;
    int bytes_written = 0;

    varint_encode(packet->packet_length, buffer, buffer_len, &bytes_tmp);
    bytes_written += bytes_tmp;

    varint_encode(packet->packet_id, buffer + bytes_written, buffer_len - bytes_written, &bytes_tmp);
    bytes_written += bytes_tmp;

    int remaining_length = packet->packet_length - bytes_tmp;
    for (int i = 0; i < remaining_length; i++, bytes_written++) {
        *(buffer + bytes_written) = *(packet->data + i);
    }

    return buffer;
}

void free_packet(ReactorPacketPtr packet) {
    if (!packet) {
        return;
    }

    if (packet->data) {
        free(packet->data);
    }

    free(packet);
}