#include "status_response.h"
#include "packet/status/out/names.h"
#include "packet/handler.h"

PacketStatusOutStatusResponse *create_status_response(uint8_t *response) {
    PacketStatusOutStatusResponse *packet = calloc(1, sizeof(PacketStatusOutStatusResponse));
    if (!packet) {
        return NULL;
    }

    packet->response = response;
    return packet;
}

ReactorPacketPtr pack_status_response(PacketStatusOutStatusResponse *response) {
    ReactorPacketPtr packet = create_empty_packet(PACKET_STATUS_OUT_STATUS_RESPONSE);
    if (!packet) {
        return NULL;
    }

    int length = u8_strlen(response->response);

    packet->packet_length += length;
    packet->data = (char *) response->response;

    return packet;
}

int send_status_response(ConnectionPtr connection, PacketStatusOutStatusResponse *response) {
    ReactorPacketPtr packed = pack_status_response(response);
    if (!packed) {
        return -1;
    }

    send_packet(connection, packed);
}