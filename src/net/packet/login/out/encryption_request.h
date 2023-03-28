#ifndef PACKET_LOGIN_OUT_ENCRYPTION_REQUEST_H
#define PACKET_LOGIN_OUT_ENCRYPTION_REQUEST_H

#include "../../../connection.h"

typedef struct packet_login_out_encryption_request {
    uint8_t *server_id;
    VarInt public_key_length;
    char *public_key;
    VarInt verify_token_length;
    char *verify_token;
} PacketLoginOutEncryptionRequest;

PacketLoginOutEncryptionRequest *create_encryption_request(VarInt pk_len, char *pk, VarInt vt_len, char *vt);

ReactorPacketPtr pack_encryption_request(PacketLoginOutEncryptionRequest *request);

int send_encryption_request(ConnectionPtr connection, PacketLoginOutEncryptionRequest *request);

#endif