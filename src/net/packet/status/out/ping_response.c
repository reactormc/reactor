#include "ping_response.h"
#include "names.h"
#include "../../packet_handler.h"
#include <arpa/inet.h>

PacketStatusOutPingResponse *create_ping_response(uint64_t response) {
    PacketStatusOutPingResponse *packet = calloc(1, sizeof(PacketStatusOutStatusResponse));
    if (!packet) {
        return NULL;
    }

    packet->payload = response;
    return packet;
}

ReactorPacketPtr pack_ping_response(PacketStatusOutPingResponse *response) {
    ReactorPacketPtr packet = create_empty_packet(PACKET_STATUS_OUT_PING_RESPONSE);
    if (!packet) {
        return NULL;
    }

    packet->packet_length += 8;

    response->payload = htonl(response->payload);
    packet->data = (char *) &response->payload;

    return packet;
}

int send_ping_response(ConnectionPtr connection, PacketStatusOutPingResponse *response) {
    ReactorPacketPtr packed = pack_ping_response(response);
    if (!packed) {
        return -1;
    }

    send_packet(connection, packed);
}