#ifndef CONNECTION_H
#define CONNECTION_H 1

#include "packet/packet.h"
#include <stdint.h>

#define STATE_HANDSHAKING 0
#define STATE_STATUS 1
#define STATE_LOGIN 2
#define STATE_PLAY 3

typedef struct connection *ConnectionPtr;
typedef struct connection {
    int remote_fd;
    int state;
} Connection;

void handle_connection(int remote_fd);

#endif
