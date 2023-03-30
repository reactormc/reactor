#include "handshake.h"
#include "util/logger.h"
#include <stdlib.h>

PacketHandshakingInHandshake *read_handshake(ReactorPacketPtr parent, byte_buffer_ptr buffer) {
    PacketHandshakingInHandshake *child = calloc(1, sizeof(PacketHandshakingInHandshake));
    if (!child) {
        return NULL;
    }

    buffer->read_varint(buffer, &child->protocol_version);
    debug("read protocol version: %d\n", child->protocol_version);

    buffer->read_string(buffer, 255, &child->server_address);
    debug("read server address: %s\n", child->server_address);

    buffer->read_ushort(buffer, &child->server_port);
    debug("read server port: %d\n", child->server_port);

    buffer->read_varint(buffer, &child->next_state);
    debug("read next state: %d\n", child->next_state);

    return child;
}

void handle_handshake(server_t *server, ConnectionPtr conn, ReactorPacketPtr packet, byte_buffer_ptr buffer) {
    PacketHandshakingInHandshake *handshake = read_handshake(packet, buffer);
    if (!handshake) {
        perror("handle_handshaking");
        free_packet(packet);
        exit(EXIT_FAILURE);
    }

    debug("handle_handshake: received handshake packet\n");

    if (handshake->next_state == STATE_STATUS) {
        debug("handle_handshake: switching to status state\n");
        conn->state = STATE_STATUS;
    } else if (handshake->next_state == STATE_LOGIN) {
        debug("handle_handshake: switching to login state\n");
        conn->state = STATE_LOGIN;
    } else {
        debug("handle_handshake: error! unknown next state intention\n");
        exit(EXIT_FAILURE);
    }

    free(handshake);
}
