#include "packet_login_handler.h"
#include "util/logger.h"

void handle_login(connection_t *conn, ReactorPacketPtr packet, byte_buffer_ptr buffer) {
    switch (packet->packet_id) {
        case PACKET_LOGIN_IN_LOGIN_START:
            handle_login_start(conn, packet, buffer);
            break;
        case PACKET_LOGIN_IN_ENCRYPTION_RESPONSE:
            handle_encryption_response(conn, packet, buffer);
            break;
        case PACKET_LOGIN_IN_LOGIN_PLUGIN_RESPONSE:
            debug("handle_login: got login plugin response packet\n");
            handle_login_plugin_response(conn, packet, buffer);
            break;
        default:
            fprintf(stderr, "Unknown status packet ID %llu\n", packet->packet_id);
    }
}