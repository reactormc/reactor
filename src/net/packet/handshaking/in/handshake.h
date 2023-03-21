#ifndef PACKET_HANDSHAKING_IN_HANDSHAKE_H
#define PACKET_HANDSHAKING_IN_HANDSHAKE_H

#include "../../../connection.h"
#include "../../../../util/unicode_string.h"
#include "../../../../util/varint.h"

typedef struct packet_handshaking_in_handshake {
    VarInt protocol_version;
    UnicodeString server_address;
    uint16_t server_port;
    VarInt next_state;
} PacketHandshakingInHandshake;

PacketHandshakingInHandshake *read_handshake(ReactorPacketPtr parent, byte_buffer_ptr buffer);

void handle_handshake(ConnectionPtr conn, ReactorPacketPtr packet, byte_buffer_ptr buffer);

#endif
