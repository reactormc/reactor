#ifndef NET_PACKET_STATUS_OUT_STATUS_RESPONSE_H
#define NET_PACKET_STATUS_OUT_STATUS_RESPONSE_H 1

#include "../../../connection.h"

typedef struct packet_status_out_status_response {
    uint8_t *response;
} PacketStatusOutStatusResponse;

PacketStatusOutStatusResponse *create_status_response(uint8_t *response);

ReactorPacketPtr pack_status_response(PacketStatusOutStatusResponse *response);

int send_status_response(ConnectionPtr connection, PacketStatusOutStatusResponse *response);

#endif