#include "packet.h"

#include "util/logger.h"
#include "util/byte_buffer.h"

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

    if (init_byte_buffer(&ret->data) != BYTE_BUFFER_INIT_SUCCESS) {
        fprintf(stderr, "create_empty_packet: failed to initialize packet data buffer\n");
        free(ret);
        return NULL;
    }

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

    VarInt packet_length = (*packet)->packet_length;
    if (!skip_packet_length) {
        buffer->read_varint(buffer, &packet_length);

        if (packet_length > buffer->remaining_length(buffer)) {
            debug("create_packet_from_header: not enough data in buffer to complete packet\n");
            buffer->roll_back(buffer, varint_encoding_length(packet_length));
            return -2;
        } else if (packet_length == 0) {
            debug("create_packet_from_header: ignoring length zero packet\n");
            return -3;
        }
    }

    VarInt packet_id;
    buffer->read_varint(buffer, &packet_id);
    (*packet)->packet_id = packet_id;

    packet_length -= varint_encoding_length(packet_id);
    (*packet)->packet_length = packet_length;

    debug("create_packet_from_header: allocating data buf\n");
    if (init_byte_buffer(&(*packet)->data) != BYTE_BUFFER_INIT_SUCCESS) {
        fprintf(stderr, "create_packet_from_header: failed to create byte buffer\n");
        return -1;
    }

    // writes the next packet_length bytes from connection buffer into packet buffer
    (*packet)->data->write_bytes((*packet)->data, packet_length, (int8_t*) buffer->next_bytes(buffer));

    return 0;
}

byte_buffer_ptr encode_packet(ReactorPacketPtr packet) {
    byte_buffer_ptr encode_buffer;
    if (init_byte_buffer(&encode_buffer) != BYTE_BUFFER_INIT_SUCCESS) {
        fprintf(stderr, "Failed to initialize packet encoding byte buffer\n");
        return NULL;
    }

    encode_buffer->write_varint(encode_buffer, packet->packet_length);
    encode_buffer->write_varint(encode_buffer, packet->packet_id);
    encode_buffer->write_bytes(encode_buffer, packet->packet_length, (int8_t*) packet->data->bytes);

    return encode_buffer;
}

void free_packet(ReactorPacketPtr packet) {
    if (!packet) {
        return;
    }

    if (packet->data) {
        free_byte_buffer(packet->data);
    }

    free(packet);
}
