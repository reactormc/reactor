#ifndef NET_PACKET_PACKET_H
#define NET_PACKET_PACKET_H 1

#include "util/byte_buffer.h"
#include "util/varint.h"

typedef struct reactor_packet *ReactorPacketPtr;
typedef struct reactor_packet {
    /**
     * Uncompressed: Length of packet_id + data
     * Compressed: Length of compressed packet_id + data
     */
    VarInt packet_length;

    /**
     * Uncompressed: Unused, set to -1.
     * Compressed: Length of uncompressed packet_id + data or 0
     */
    VarInt data_length;

    /**
     * Uncompressed: packet_id
     * Compressed: zlib compressed packet_id
     */
    VarInt packet_id;

    /*
     * Uncompressed: Ready to use chunk of data
     * Compressed: zlib compressed packet data
     */
    byte_buffer_ptr data;
} ReactorPacket;

ReactorPacketPtr create_empty_packet(int packet_id);

int create_packet_from_header(byte_buffer_ptr buffer, int compressed, ReactorPacketPtr *packet);

byte_buffer_ptr encode_packet(ReactorPacketPtr packet);

void free_packet(ReactorPacketPtr packet);

#endif