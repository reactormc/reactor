#include "encryption_request.h"
#include "names.h"
#include "net/packet/packet_handler.h"
#include "util/unicode_string.h"
#include "util/logger.h"

#include <stdlib.h>
#include <sys/random.h>

PacketLoginOutEncryptionRequest *create_encryption_request(server_t *server) {
    PacketLoginOutEncryptionRequest *packet = calloc(1, sizeof(PacketLoginOutEncryptionRequest));
    if (!packet) {
        return NULL;
    }

    packet->server_id = calloc(20, sizeof(ucs4_t));
    if (!packet->server_id) {
        fprintf(stderr, "reactor: failed to allocate space for server id\n");
        free(packet);
        return NULL;
    }

    u8_strncpy(packet->server_id, server->server_id, 20);

    packet->public_key = (int8_t*) server->public_key_x509;
    packet->public_key_length = (VarInt) server->public_key_x509_len;

    debug("reactor: generating proto encryption verify token\n");
    packet->verify_token_length = 16;
    packet->verify_token = calloc(packet->verify_token_length, sizeof(int8_t));
    getrandom(packet->verify_token, packet->verify_token_length, 0);

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

    packet->data->write_lp_string(packet->data, (int) u8_strlen(request->server_id), 20, encode_ntus_to_lpus(request->server_id, 20));
    packet->data->write_varint(packet->data, request->public_key_length);
    packet->data->write_bytes(packet->data, request->public_key_length, request->public_key);
    packet->data->write_varint(packet->data, request->verify_token_length);
    packet->data->write_bytes(packet->data, request->verify_token_length, request->verify_token);

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