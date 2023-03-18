#ifndef CONNECTION_H
#define CONNECTION_H 1

#include "./packet/types.h"

#define PACKET_BUFFER_SIZE 4096

#define STATE_HANDSHAKING 0
#define STATE_STATUS 1
#define STATE_LOGIN 2
#define STATE_PLAY 3

typedef struct connection *ConnectionPtr;
typedef struct connection {
    int remote_fd;
    int state;
} Connection;

ConnectionPtr create_connection(int remote_fd);

void handle_connection(ConnectionPtr conn);

void handle_packet(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset);

#endif
