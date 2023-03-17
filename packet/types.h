#ifndef PACKET_TYPES_H
#define PACKET_TYPES_H 1

#include "datatypes.h"

/* ReactorPacket, ReactorPacketPtr {{{1 */
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
/* }}}1 */

/* Handshaking State {{{1 */
typedef struct packet_handshaking_in_handshake {
    VarInt protocol_version;
    UnicodeString server_address;
    uint16_t server_port;
    VarInt next_state;
} PacketHandshakingInHandshake;
/* }}}1 */

/* Status State {{{1*/
/* Clientbound {{{2 */
typedef struct packet_status_out_status_response {
    uint8_t *response;
} PacketStatusOutStatusResponse;

typedef struct packet_status_out_ping_response {
   long payload; 
} PacketStatusOutPingResponse;
/* }}}2 */
/* Serverbound {{{2 */
// The status request packet has no fields to read, so no code for it.
typedef struct packet_status_in_ping_request {
    long payload;
} PacketStatusInPingRequest;
/* }}}2 */
/* }}}1 */

#endif
