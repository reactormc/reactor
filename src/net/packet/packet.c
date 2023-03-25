#include "packet.h"

#include "../../util/logger.h"

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

/**
 * Reads one packet from the given buffer of length {@code length} starting at offset {@code offset}.
 * Compressed format, set {@code compressed = 1}. Regular format, set {@code compressed = 0}.
 *
 * @param buffer the byte array buffer containing packet data
 * @param length the length of the byte array
 * @param offset the offset into the byte array
 * @param compressed whether to use compressed packet format
 * @param packet pointer to packet we are creating
 *
 * @return -2: no room for packet, nothing has been read (alternatively, no more packets seem to be in buffer)
 *         -1: calloc failed (fatal)
 *          0: all good! one packet read
 */
int create_packet_from_header(byte_buffer_ptr buffer, int compressed, ReactorPacketPtr *packet) {
    int skip_packet_length = 1;

    if (!*packet) {
        skip_packet_length = 0;

        *packet = calloc(1, sizeof(ReactorPacket));
        if (!*packet) {
            perror("create_packet_from_header: packet calloc");
            return -1;
        }
    }

    if (buffer->remaining_length(buffer) < VARINT_MAX_LEN) {
        debug("create_packet_from_header: cannot read a full varint\n");
        return -2;
    }

    VarInt packet_length = (*packet)->packet_length;
    if (!skip_packet_length) {
        buffer->read_varint(buffer, VARINT_MAX_LEN, &packet_length);

        if (packet_length == 0) {
            debug("create_packet_from_header: read packet length 0\n");
            return -2;
        } else if (packet_length > buffer->remaining_length(buffer)) {
            buffer->roll_back(buffer, varint_encoding_length(packet_length));
            return -2;
        }
    }

    VarInt packet_id;
    buffer->read_varint(buffer, VARINT_MAX_LEN, &packet_id);
    (*packet)->packet_id = packet_id;

    packet_length = buffer->remaining_length(buffer);
    (*packet)->packet_length = packet_length;

    debug("create_packet_from_header: allocating data buf of length %d\n", packet_length);
    char *data_buf = calloc(packet_length, sizeof(char));
    if (!data_buf) {
        perror("create_packet_from_header: data_buf calloc");
        return -1;
    }

    memcpy(data_buf, buffer->next_bytes(buffer), packet_length);
    (*packet)->data = data_buf;
    buffer->read_offset += (int) packet_length;

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
