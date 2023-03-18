#ifndef PACKET_PACKET_H
#define PACKET_PACKET_H 1

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
    char *data;
} ReactorPacket;

ReactorPacketPtr create_empty_packet(int packet_id);
int create_packet_from_header(char *buffer, int length, int *offset, int compressed, ReactorPacketPtr *packet);
char *encode_packet(ReactorPacketPtr packet);
void free_packet(ReactorPacketPtr packet);

#endif