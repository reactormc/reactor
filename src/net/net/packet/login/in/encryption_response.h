#ifndef NET_PACKET_LOGIN_IN_ENCRYPTION_RESPONSE_H
#define NET_PACKET_LOGIN_IN_ENCRYPTION_RESPONSE_H 1

#include "net/connection.h"

typedef struct packet_login_in_encryption_response {
    VarInt shared_secret_length;
    int8_t *shared_secret;
    VarInt  verify_token_length;
    int8_t *verify_token;
} PacketLoginInEncryptionResponse;

void handle_encryption_response(server_t *server, ConnectionPtr conn, ReactorPacketPtr packet, byte_buffer_ptr buffer);

#endif