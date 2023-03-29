#include "ping_request.h"
#include "../out/ping_response.h"

#include <stdlib.h>
#include <stdio.h>

PacketStatusInPingRequest *read_ping_request(ReactorPacketPtr parent, byte_buffer_ptr buffer) {
    PacketStatusInPingRequest *child = calloc(1, sizeof(PacketStatusInPingRequest));
    if (!child) {
        return NULL;
    }

    int64_t payload;
    buffer->read_long(buffer, &payload);
    child->payload = payload;

    return child;
}

void handle_ping_request(server_t *server, ConnectionPtr conn, ReactorPacketPtr packet, byte_buffer_ptr buffer) {
    PacketStatusInPingRequest *ping_request = read_ping_request(packet, buffer);
    if (!ping_request) {
        perror("handle_ping_request");
        free_packet(packet);
        exit(EXIT_FAILURE);
    }

    send_ping_response(conn, create_ping_response(ping_request->payload));

    free(ping_request);
}