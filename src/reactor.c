#include "server.h"
#include "util/logger.h"

#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#ifndef PORT
#define PORT "25565"
#endif

void sigchld_handler(int ignored) {
    int saved_errno = errno;
    debug("reactor: received sigchld, waiting for exit\n");
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

int main(int argc, char **argv) {
    debug("reactor: starting up\n");

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status;
    struct addrinfo *server_info;
    if ((status = getaddrinfo(NULL, PORT, &hints, &server_info)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    int server_socket_fd, yes = 1;
    struct addrinfo *result;
    for (result = server_info; result; result = result->ai_next) {
        if ((server_socket_fd = socket(result->ai_family, result->ai_socktype,
                                       result->ai_protocol)) == -1) {

            perror("socket");
            continue;
        }

        if (setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {

            perror("setsockopt");
            exit(1);
        }

        if (bind(server_socket_fd, result->ai_addr, result->ai_addrlen) == -1) {
            perror("bind");
            close(server_socket_fd);
            continue;
        }

        break;
    }

    freeaddrinfo(server_info);

    if (!result) {
        fprintf(stderr, "reactor: failed to bind\n");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket_fd, 10) == -1) {
        perror("listen");
        exit(1);
    }

    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    start_server((uint8_t *) "reactormc", server_socket_fd);

    return 0;
}