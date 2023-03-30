#ifndef NET_PACKET_LOGIN_OUT_ENCRYPTION_REQUEST_H
#define NET_PACKET_LOGIN_OUT_ENCRYPTION_REQUEST_H

#include "net/connection.h"

typedef struct packet_login_out_encryption_request {
    uint8_t *server_id;
    VarInt public_key_length;
    int8_t *public_key;
    VarInt verify_token_length;
    int8_t *verify_token;
} PacketLoginOutEncryptionRequest;

PacketLoginOutEncryptionRequest *create_encryption_request(server_t *server);

ReactorPacketPtr pack_encryption_request(PacketLoginOutEncryptionRequest *request);

int send_encryption_request(ConnectionPtr connection, PacketLoginOutEncryptionRequest *request);

#endif