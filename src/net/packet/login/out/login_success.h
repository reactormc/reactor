#ifndef NET_PACKET_LOGIN_OUT_LOGIN_SUCCESS_H
#define NET_PACKET_LOGIN_OUT_LOGIN_SUCCESS_H 1

#include "../../../connection.h"

typedef struct packet_login_out_login_success_property {
    uint8_t *name;
    uint8_t *value;
    uint8_t is_signed;
    uint8_t *signature;
} PacketLoginOutLoginSuccessProperty;

typedef struct packet_login_out_login_success {
    uuid_t uuid;
    uint8_t *username;
    VarInt number_of_properties;
    PacketLoginOutLoginSuccessProperty *properties;
} PacketLoginOutLoginSuccess;

PacketLoginOutLoginSuccess *create_login_success(server_t *server);

ReactorPacketPtr pack_login_success(PacketLoginOutLoginSuccess *request);

int send_login_success(ConnectionPtr connection, PacketLoginOutLoginSuccess *login_success);

#endif