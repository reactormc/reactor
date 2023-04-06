#include "packet_handshaking_handler.h"
#include "names.h"
#include "types.h"

void handle_handshaking(connection_t *conn, ReactorPacketPtr packet, byte_buffer_ptr buffer) {
    if (packet->packet_id == PACKET_HANDSHAKING_IN_HANDSHAKE) {
        handle_handshake(conn, packet, buffer);
    }
}