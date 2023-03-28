#include "encryption_request.h"
#include "names.h"
#include "../../packet_handler.h"
#include "../../../../util/unicode_string.h"

#include <stdlib.h>

PacketLoginOutEncryptionRequest *create_encryption_request(VarInt pk_len, char *pk, VarInt vt_len, char *vt) {
    PacketLoginOutEncryptionRequest *packet = calloc(1, sizeof(PacketLoginOutEncryptionRequest));
    if (!packet) {
        return NULL;
    }

    packet->server_id = encode_ntls_to_lpus("BestSerburEbur", 20);
    packet->public_key_length = pk_len;
    packet->public_key = pk;
    packet->verify_token_length = vt_len;
    packet->verify_token = vt;

    return packet;
}

ReactorPacketPtr pack_encryption_request(PacketLoginOutEncryptionRequest *request) {
    ReactorPacketPtr packet = create_empty_packet(PACKET_LOGIN_OUT_ENCRYPTION_REQUEST);
    if (!packet) {
        return NULL;
    }

    packet->packet_length += u8_strlen(request->server_id);
    packet->packet_length += request->public_key_length + varint_encoding_length(request->public_key_length);
    packet->packet_length += request->verify_token_length + varint_encoding_length(request->verify_token_length);
    packet->data = (char *) request;

    return packet;
}

int send_encryption_request(ConnectionPtr connection, PacketLoginOutEncryptionRequest *request) {
    ReactorPacketPtr packed = pack_encryption_request(request);
    if (!packed) {
        return -1;
    }

    send_packet(connection, packed);

    return 0;
}