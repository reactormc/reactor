#include "handshake.h"
#include "util/byte_utils.h"
#include "util/logger.h"
#include <json-c/json.h>

PacketHandshakingInHandshake *read_handshake(ReactorPacketPtr parent, int *buffer_offset) {
    PacketHandshakingInHandshake *child = calloc(1, sizeof(PacketHandshakingInHandshake));
    if (!child) {
        return NULL;
    }

    int offset = 0;

    child->protocol_version = varint_decode_offset(parent->data, parent->packet_length, &offset);
    child->server_address = read_unicode_string(parent->data, parent->packet_length - offset, &offset);
    child->server_port = read_uint16(parent->data, &offset);
    child->next_state = varint_decode_offset(parent->data, parent->packet_length - offset, &offset);

    *buffer_offset += offset;

    return child;
}

void handle_handshake(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset) {
    PacketHandshakingInHandshake *handshake = read_handshake(packet, read_buffer_offset);
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
    }

    free(handshake);
}