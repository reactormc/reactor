#ifndef PACKET_STATUS_IN_PING_REQUEST_H
#define PACKET_STATUS_IN_PING_REQUEST_H 1

#include "../../../connection.h"
#include <stdint.h>

typedef struct packet_status_in_ping_request {
    int64_t payload;
} PacketStatusInPingRequest;

PacketStatusInPingRequest *read_ping_request(ReactorPacketPtr parent, byte_buffer_ptr buffer);

void handle_ping_request(ConnectionPtr conn, ReactorPacketPtr packet, byte_buffer_ptr buffer);

#endif