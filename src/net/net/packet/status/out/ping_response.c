#include "ping_response.h"
#include "names.h"
#include "net/packet/packet_handler.h"

#include <stdlib.h>

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

    response->payload = htobe64(response->payload);
    packet->data->write_bytes(packet->data, 8, (int8_t*) &response->payload);

    return packet;
}

int send_ping_response(connection_t *conn, PacketStatusOutPingResponse *response) {
    ReactorPacketPtr packed = pack_ping_response(response);
    if (!packed) {
        return -1;
    }

    send_packet(conn, packed);
}