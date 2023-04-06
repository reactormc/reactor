#include "status_response.h"
#include "names.h"
#include "net/packet/packet_handler.h"
#include "util/unicode_string.h"

#include <stdlib.h>

PacketStatusOutStatusResponse *create_status_response(char *json_response) {
    PacketStatusOutStatusResponse *packet = calloc(1, sizeof(PacketStatusOutStatusResponse));
    if (!packet) {
        return NULL;
    }

    packet->response = encode_ntls_to_lpus(json_response, 32767);
    packet->response_len = strlen(json_response);

    return packet;
}

ReactorPacketPtr pack_status_response(PacketStatusOutStatusResponse *response) {
    ReactorPacketPtr packet = create_empty_packet(PACKET_STATUS_OUT_STATUS_RESPONSE);
    if (!packet) {
        return NULL;
    }

    packet->packet_length += response->response_len;
    packet->data->write_bytes(packet->data, response->response_len, (int8_t*) response->response);

    return packet;
}

int send_status_response(connection_t *conn, PacketStatusOutStatusResponse *response) {
    ReactorPacketPtr packed = pack_status_response(response);
    if (!packed) {
        return -1;
    }

    send_packet(conn, packed);

    return 0;
}
