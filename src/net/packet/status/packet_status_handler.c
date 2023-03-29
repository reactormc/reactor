#include "packet_status_handler.h"
#include "../../../util/logger.h"

#include <stdio.h>

void handle_status(server_t *server, ConnectionPtr conn, ReactorPacketPtr packet, byte_buffer_ptr buffer) {
    switch (packet->packet_id) {
        case PACKET_STATUS_IN_STATUS_REQUEST:
            debug("handle_status: got status request\n");
            handle_status_request(server, conn, packet, buffer);
            break;
        case PACKET_STATUS_IN_PING_REQUEST:
            debug("handle_status: got ping request\n");
            handle_ping_request(server, conn, packet, buffer);
            break;
        default:
            fprintf(stderr, "Unknown status packet ID %llu\n", packet->packet_id);
    }
}