#ifndef PACKET_LOGIN_IN_LOGIN_START_H
#define PACKET_LOGIN_IN_LOGIN_START_H 1

#include "../../../connection.h"

typedef struct packet_login_in_login_start {
    uint8_t *player_name;
    unsigned char has_player_uuid;
    uuid_t player_uuid;
} PacketLoginInLoginStart;

void handle_login_start(ConnectionPtr conn, ReactorPacketPtr packet, byte_buffer_ptr buffer);

#endif