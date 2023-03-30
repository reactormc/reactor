#ifndef NET_PACKET_STATUS_OUT_PING_RESPONSE_H
#define NET_PACKET_STATUS_OUT_PING_RESPONSE_H 1

#include "net/connection.h"

#include <stdint.h>

typedef struct packet_status_out_ping_response {
    uint64_t payload;
} PacketStatusOutPingResponse;

PacketStatusOutPingResponse *create_ping_response(uint64_t response);

ReactorPacketPtr pack_ping_response(PacketStatusOutPingResponse *response);

int send_ping_response(ConnectionPtr connection, PacketStatusOutPingResponse *response);

#endif