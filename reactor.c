#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "connection.h"
#include "include/varint.h"
#include "packet/types.h"
#include "packet/reader.h"

#ifndef PORT
#define PORT "25565"
#endif

/* sigchld_handler(int): void {{{1 */
void sigchld_handler(int signal) {
    int saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}
/* }}}1 */

/* create_server_hints(struct addrinfo*): void {{{1 */
void create_server_hints(struct addrinfo *hints) {
    if (!hints) {
        return;
    }

    memset(hints, 0, sizeof(*hints));
    hints->ai_family = AF_UNSPEC;
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_flags = AI_PASSIVE;
}
/* }}}1 */

/* get_in_addr(struct sockaddr*): void* {{{1 */
void *get_in_addr(struct sockaddr *addr) {
    if (addr->sa_family == AF_INET) {
        return &(((struct sockaddr_in*) addr)->sin_addr);
    }

    return &(((struct sockaddr_in6*) addr)->sin6_addr);
}
/* }}}1 */

/* main(int, char**): int {{{1 */
int main(int argc, char **argv) {
    struct addrinfo hints;
    create_server_hints(&hints);

    int status;
    struct addrinfo *server_info;
    if ((status = getaddrinfo(NULL, PORT, &hints, &server_info)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    int sock_fd, yes = 1;
    struct addrinfo *result;
    for (result = server_info; result; result = result->ai_next) {
        if ((sock_fd = socket(result->ai_family, result->ai_socktype, 
                        result->ai_protocol)) == -1) {

            perror("socket");
            continue;
        }

        if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, 
                    sizeof(int)) == -1) {

            perror("setsockopt");
            exit(1);
        }

        if (bind(sock_fd, result->ai_addr, result->ai_addrlen) == -1) {
            perror("bind");
            close(sock_fd);
            continue;
        }

        break;
    }

    freeaddrinfo(server_info);

    if (!result) {
        fprintf(stderr, "reactor: failed to bind\n");
        exit(EXIT_FAILURE);
    }

    if (listen(sock_fd, 10) == -1) {
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

    printf("reactor: waiting for connections...\n");

    while (1) {
        struct sockaddr_storage their_addr;
        socklen_t sin_size = sizeof(their_addr);

        int remote_fd = accept(sock_fd, (struct sockaddr *) &their_addr, 
                &sin_size);

        if (remote_fd == -1) {
            perror("accept");
            continue;
        }

        char s[INET6_ADDRSTRLEN];
        inet_ntop(their_addr.ss_family, 
                get_in_addr((struct sockaddr*) &their_addr), s, sizeof(s));

        printf("reactor: got connection from %s\n", s);

        int fork_status;
        if ((fork_status = fork()) == -1) {
            perror("reactor: fork");
            exit(1);
        }

        if (fork_status == 0) {
            close(sock_fd);
            handle_connection(create_connection(remote_fd));
        } else {
            close(remote_fd);
        }
    }

    return 0;
}
/* }}}1 */