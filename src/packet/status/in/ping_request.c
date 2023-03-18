#include "ping_request.h"
#include "packet/status/out/ping_response.h"
#include "connection.h"
#include "util/byte_utils.h"
#include "util/logger.h"

#include <stdlib.h>
#include <stdio.h>

PacketStatusInPingRequest *read_ping_request(ReactorPacketPtr parent, int *buffer_offset) {
    PacketStatusInPingRequest *child = calloc(1, sizeof(PacketStatusInPingRequest));
    if (!child) {
        return NULL;
    }

    int offset = 0;

    child->payload = read_uint64(parent->data, &offset);

    *buffer_offset += offset;

    return child;
}

void handle_ping_request(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset) {
    PacketStatusInPingRequest *ping_request = read_ping_request(packet, read_buffer_offset);
    if (!ping_request) {
        perror("handle_handshaking");
        free_packet(packet);
        exit(EXIT_FAILURE);
    }

    send_ping_response(conn, create_ping_response(ping_request->payload));

    free(ping_request);
}