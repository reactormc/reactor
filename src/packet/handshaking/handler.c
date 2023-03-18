#include "packet/handshaking/handler.h"
#include "packet/handshaking/names.h"
#include "packet/handshaking/types.h"

void handle_handshaking(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset) {
    if (packet->packet_id == PACKET_HANDSHAKING_IN_HANDSHAKE) {
        handle_handshake(conn, packet, read_buffer_offset);
    }
}