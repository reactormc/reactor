#include "server.h"

#include "net/connection.h"
#include "util/logger.h"
#include "util/unicode_string.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <tommath.h>

static void server_generate_rsa_keypair(server_t *server) {
    debug("server_generate_rsa_keypair: registering prng...\n");
    if (register_prng(&sprng_desc) == -1) {
        fprintf(stderr, "reactor: failed to register SPRNG\n");
        exit(EXIT_FAILURE);
    }

    // TODO: here, set math lib descriptor ?

    debug("server_generate_rsa_keypair: generating 1024-bit rsa key...\n");
    rsa_key key;
    int err;
    if ((err = rsa_make_key(NULL, find_prng("sprng"), 1024/8, 65537, &key)) != CRYPT_OK) {
        fprintf(stderr, "reactor: failed to create rsa keypair: %s\n", error_to_string(err));
    }

    debug("server_generate_rsa_keypair: extracting X.509 public key...\n");
    unsigned long key_size = 1024;
    unsigned char exported_key[key_size];
    if (rsa_export(exported_key, &key_size, PK_STD | PK_PUBLIC, &server->rsa_key) != 0) {
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

static void server_start_connection_loop(server_t *server, int server_socket_fd) {
    while (1) {
        struct sockaddr_storage their_addr;
        socklen_t sin_size = sizeof(their_addr);

        int remote_fd = accept(server_socket_fd, (struct sockaddr *) &their_addr,
                               &sin_size);

        if (remote_fd == -1) {
            perror("accept");
            continue;
        }

        char s[INET6_ADDRSTRLEN];
        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *) &their_addr), s, sizeof(s));

        printf("reactor: got connection from %s\n", s);

        int fork_status;
        if ((fork_status = fork()) == -1) {
            perror("reactor: fork");
            exit(1);
        }

        if (fork_status == 0) {
            close(server_socket_fd);
            printf("reactor: starting child process to manage connection\n");
            handle_connection(server, remote_fd);
        } else {
            close(remote_fd);
        }
    }
}

server_t *start_server(uint8_t *server_id, int server_socket_fd) {
    printf("reactor: initializing server\n");

    server_t *server = calloc(1, sizeof(server_t));
    if (!server) {
        fprintf(stderr, "reactor: failed to allocate space for server struct\n");
        return NULL;
    }

    server->server_id = calloc(20, sizeof(ucs4_t));
    if (!server->server_id) {
        fprintf(stderr, "reactor: failed to allocate space for server id\n");
        free(server);
        return NULL;
    }

    u8_strncpy(server->server_id, server_id, 20);

    printf("reactor: generating rsa keypair...\n");
    server_generate_rsa_keypair(server);

    printf("reactor: waiting for connections...\n");
    server_start_connection_loop(server, server_socket_fd);
}