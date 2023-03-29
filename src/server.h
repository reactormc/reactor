#ifndef SERVER_H
#define SERVER_H 1

#include <stdint.h>
#include <tomcrypt.h>

typedef struct server_t {
    uint8_t *server_id;
    rsa_key rsa_key;
    int8_t *public_key_x509;
    uint8_t public_key_x509_len;
} server_t;

server_t *start_server(uint8_t *server_id, int server_socket_fd);

#endif
