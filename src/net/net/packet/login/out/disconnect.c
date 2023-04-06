#include "disconnect.h"
#include "names.h"
#include "net/packet/packet_handler.h"
#include "util/logger.h"
#include "util/unicode_string.h"

#include <stdlib.h>
#include <string.h>

PacketLoginOutDisconnect *create_disconnect(char *reason) {
    PacketLoginOutDisconnect *packet = calloc(1, sizeof(PacketLoginOutDisconnect));
    if (!packet) {
        return NULL;
    }

    debug("create_disconnect: creating disconnect with reason %s (len %d)\n", reason, strlen(reason));

    packet->reason_len = strlen(reason);

    packet->reason = calloc(packet->reason_len + varint_encoding_length(packet->reason_len), sizeof(char));
    if (!packet->reason) {
        free(packet);
        return NULL;
    }

    // TODO: magic number
    memcpy(
            packet->reason,
            encode_ntls_to_lpus(reason, 262144),
            packet->reason_len + varint_encoding_length(packet->reason_len)
    );

    debug("create_disconnect: packet->reason contains %s\n", packet->reason);

    return packet;
}

ReactorPacketPtr pack_disconnect(PacketLoginOutDisconnect *disconnect) {
    ReactorPacketPtr packet = create_empty_packet(PACKET_LOGIN_OUT_DISCONNECT);
    if (!packet) {
        return NULL;
    }

    debug("pack_disconnect: packing disconnect\n");

    packet->packet_length += disconnect->reason_len + varint_encoding_length(disconnect->reason_len);

    packet->data = calloc(disconnect->reason_len + varint_encoding_length(disconnect->reason_len), sizeof(ucs4_t));
    if (!packet->data) {
        free(packet);
        return NULL;
    }

    memcpy(packet->data, disconnect->reason, disconnect->reason_len + varint_encoding_length(disconnect->reason_len));

    debug("pack_disconnect: packet->packet_id = %d (encoding length %d)\n", packet->packet_id, varint_encoding_length(packet->packet_id));
    debug("pack_disconnect: disconnect->reason_len = %d (encoding length %d)\n", disconnect->reason_len, varint_encoding_length(disconnect->reason_len));
    debug("pack_disconnect: packet->packet_length = %d\n", packet->packet_length);
    debug("pack_disconnect: packet->data contains %s\n", packet->data);

    return packet;
}

int send_disconnect(connection_t *conn, PacketLoginOutDisconnect *disconnect) {
    ReactorPacketPtr packed = pack_disconnect(disconnect);
    if (!packed) {
        return -1;
    }

    send_packet(conn, packed);

    return 0;
}