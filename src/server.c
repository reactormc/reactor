#include "server.h"

#include "net/connection.h"
#include "util/logger.h"
#include "util/unicode_string.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

static void server_generate_rsa_keypair(server_t *server) {
    ltc_mp = ltm_desc;

    debug("server_generate_rsa_keypair: registering prng...\n");
    if (register_prng(&sprng_desc) == -1) {
        fprintf(stderr, "reactor: failed to register SPRNG\n");
        exit(EXIT_FAILURE);
    }

    debug("server_generate_rsa_keypair: generating 1024-bit rsa key...\n");
    rsa_key key;
    int err;
    if ((err = rsa_make_key(NULL, find_prng("sprng"), 1024/8, 65537, &key)) != CRYPT_OK) {
        fprintf(stderr, "reactor: failed to create rsa keypair: %s\n", error_to_string(err));
    }

    debug("server_generate_rsa_keypair: key generated.\n");
    server->rsa_key = key;

    debug("server_generate_rsa_keypair: extracting X.509 public key...\n");
    uint64_t key_size = 1024;
    uint8_t exported_key[key_size];
    memset(exported_key, 0, key_size);
    if (rsa_export(exported_key, &key_size, PK_STD | PK_PUBLIC, &server->rsa_key) != CRYPT_OK) {
        fprintf(stderr, "reactor: failed to export rsa public key\n");
        exit(EXIT_FAILURE);
    }

    server->rsa_key = key;

    server->public_key_x509 = calloc(key_size, sizeof(uint8_t));
    if (!server->public_key_x509) {
        fprintf(stderr, "reactor: failed to allocate space for public key\n");
        exit(EXIT_FAILURE);
    }

    memcpy(server->public_key_x509, exported_key, key_size);

    server->public_key_x509_len = key_size;
}

static void *get_in_addr(struct sockaddr *addr) {
    if (addr->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) addr)->sin_addr);
    }

    return &(((struct sockaddr_in6 *) addr)->sin6_addr);
}

static void accept_connection(server_t *server) {
    struct sockaddr_storage their_addr;
    socklen_t sin_size = sizeof(their_addr);

    int remote_fd = accept(server->socket_fd, (struct sockaddr *) &their_addr,
                           &sin_size);

    if (remote_fd == -1) {
        perror("accept");
        return;
    }

    char s[INET6_ADDRSTRLEN];
    inet_ntop(their_addr.ss_family,
              get_in_addr((struct sockaddr *) &their_addr), s, sizeof(s));

    printf("reactor: got connection from %s\n", s);

    connection_t *conn = calloc(1, sizeof(connection_t));
    if (!conn) {
        fprintf(stderr, "reactor: failed to allocate connection in heap\n");
        return;
    }

    conn->server = server;
    conn->remote_fd = remote_fd;

    pthread_t packet_handler_thread;
    pthread_create(&packet_handler_thread, NULL, handle_connection, conn);
}

static void *tick_loop(void *arg) {
    server_t *server = (server_t*) arg;

    printf("reactor: waiting for connections...\n");
    while (server->shutdown == 0) {
        accept_connection(server);
    }

    return NULL;
}

server_t *start_server(uint8_t *server_id, int server_socket_fd) {
    printf("reactor: initializing server\n");

    server_t *server = calloc(1, sizeof(server_t));
    if (!server) {
        fprintf(stderr, "reactor: failed to allocate space for server struct\n");
        return NULL;
    }

    server->socket_fd = server_socket_fd;
    server->shutdown = 0;

    server->server_id = calloc(20, sizeof(ucs4_t));
    if (!server->server_id) {
        fprintf(stderr, "reactor: failed to allocate space for server id\n");
        free(server);
        return NULL;
    }

    u8_strncpy(server->server_id, server_id, 20);

    printf("reactor: generating rsa keypair...\n");
    server_generate_rsa_keypair(server);

    printf("reactor: starting server tick loop\n");
    pthread_t server_tick_loop;
    pthread_create(&server_tick_loop, NULL, tick_loop, server);
    pthread_join(server_tick_loop, NULL);

    return NULL;
}