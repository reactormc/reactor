#ifndef PACKET_STATUS_IN_PING_REQUEST_H
#define PACKET_STATUS_IN_PING_REQUEST_H 1

#include "connection.h"
#include <stdint.h>

typedef struct packet_status_in_ping_request {
    uint64_t payload;
} PacketStatusInPingRequest;

PacketStatusInPingRequest *read_ping_request(ReactorPacketPtr parent, int *read_buffer_offset);
void handle_ping_request(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset);

#endif