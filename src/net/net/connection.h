#ifndef NET_CONNECTION_H
#define NET_CONNECTION_H 1

#include "net/packet/packet.h"
#include "../../server.h"

#include <stdint.h>

#define STATE_HANDSHAKING 0
#define STATE_STATUS 1
#define STATE_LOGIN 2
#define STATE_PLAY 3

typedef struct connection_t *ConnectionPtr;
typedef struct connection_t {
    server_t *server;
    int remote_fd;

    uint8_t state;
    uint8_t is_encrypted;

    int8_t *shared_secret;

    uint8_t *profile_id;
    uint8_t *username;
    uuid_t unique_id;

    int (*set_profile_id)(ConnectionPtr self, uint8_t *profile_id);
    int (*set_username)(ConnectionPtr self, uint8_t *username);
    int (*set_unique_id)(ConnectionPtr self, uuid_t unique_id);
} connection_t;


void *handle_connection(void *arg);

#endif
