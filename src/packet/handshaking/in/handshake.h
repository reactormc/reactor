#ifndef PACKET_HANDSHAKING_IN_HANDSHAKE_H
#define PACKET_HANDSHAKING_IN_HANDSHAKE_H

#include "util/unicode_string.h"
#include "util/varint.h"
#include "packet/packet.h"
#include "connection.h"

typedef struct packet_handshaking_in_handshake {
    VarInt protocol_version;
    UnicodeString server_address;
    uint16_t server_port;
    VarInt next_state;
} PacketHandshakingInHandshake;

PacketHandshakingInHandshake *read_handshake(ReactorPacketPtr parent, int *read_buffer_offset);
void handle_handshake(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset);

#endif
