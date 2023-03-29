#ifndef NET_PACKET_LOGIN_OUT_DISCONNECT_H
#define NET_PACKET_LOGIN_OUT_DISCONNECT_H 1

#include "../../../connection.h"

typedef struct packet_login_out_disconnect {
    uint8_t *reason;
    unsigned int reason_len;
} PacketLoginOutDisconnect;

PacketLoginOutDisconnect *create_disconnect(char *reason);

ReactorPacketPtr pack_disconnect(PacketLoginOutDisconnect *disconnect);

int send_disconnect(ConnectionPtr connection, PacketLoginOutDisconnect *disconnect);

#endif