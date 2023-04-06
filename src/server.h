#ifndef SERVER_H
#define SERVER_H 1

#include <stdint.h>
#include <pthread.h>
#include <tommath.h>
#include <tomcrypt.h>

typedef struct server_t {
    int socket_fd;
    uint8_t shutdown;

    uint8_t *server_id;
    rsa_key rsa_key;
    uint8_t *public_key_x509;
    uint8_t public_key_x509_len;
} server_t;

server_t *start_server(uint8_t *server_id, int server_socket_fd);

#endif
