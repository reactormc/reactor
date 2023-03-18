#ifndef PACKET_HANDSHAKING_IN_HANDSHAKE_H
#define PACKET_HANDSHAKING_IN_HANDSHAKE_H 1

#include "include/varint.h"
#include "packet/types.h"

typedef struct packet_handshaking_in_handshake {
    VarInt protocol_version;
    UnicodeString server_address;
    uint16_t server_port;
    VarInt next_state;
} PacketHandshakingInHandshake;

#endif
