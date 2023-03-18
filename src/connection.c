#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "connection.h"
#include "util/logger.h"
#include "packet/handler.h"

static ssize_t read_buffer_size = 0;
static char read_buffer[PACKET_BUFFER_SIZE] = {0};

int read_network(ConnectionPtr conn) {
    ssize_t bytes_read;

    if ((bytes_read = recv(conn->remote_fd, read_buffer + read_buffer_size,
                           PACKET_BUFFER_SIZE - read_buffer_size, 0)) == -1) {
        perror("reactor: recv");
        return -1;
    }

    if (bytes_read == 0) {
        printf("reactor: client disconnected\n");
        return 1;
    }

    read_buffer_size += bytes_read;

    return 0;
}

void handle_connection(int remote_fd) {
    printf("reactor: client connected\n");

    ConnectionPtr conn = calloc(1, sizeof(Connection));
    if (!conn) {
        perror("create_connection - calloc");
        exit(EXIT_FAILURE);
    }

    conn->remote_fd = remote_fd;
    conn->state = STATE_HANDSHAKING;

    ReactorPacketPtr packet = NULL;
    int status, next_read_offset = 0;

    while (1) {
        if (read_buffer_size == 0) {
            debug("handle_connection: reading packet(s) from network into buffer\n");

            status = read_network(conn);

            if (status == -1) {
                free(conn);
                exit(EXIT_FAILURE);
            } else if (status == 1) {
                free(conn);
                exit(EXIT_SUCCESS);
            }

            debug("handle_connection: done reading from network\n");
            continue;
        }

        while (1) {
            debug("handle_connection: reading packet(s) from buffer\n");

            status = create_packet_from_header(read_buffer, PACKET_BUFFER_SIZE, &next_read_offset, 0, &packet);
            if (packet == NULL) {
                debug("handle_connection: null packet, skipping...");

                if (next_read_offset > 0) {
                    memmove(read_buffer, read_buffer + next_read_offset, read_buffer_size - next_read_offset);
                    read_buffer_size -= next_read_offset;
                    next_read_offset = 0;
                }

                continue;
            }

            switch (status) {
                case -2:
                    debug("handle_connection: read_packet reported partial packet, preserving buffer\n");
                    if (next_read_offset > 0) {
                        memmove(read_buffer, read_buffer + next_read_offset, read_buffer_size - next_read_offset);
                        read_buffer_size -= next_read_offset;
                        next_read_offset = 0;
                    }
                    break;
                case -1:
                    debug("handle_connection: failed to allocate space for packet\n");
                    exit(EXIT_FAILURE);
                case 0:
                    debug("handle_connection: handling packet\n");
                    handle_packet(conn, packet, &next_read_offset);
                    free_packet(packet);
                    if (next_read_offset > 0) {
                        memmove(read_buffer, read_buffer + next_read_offset, read_buffer_size - next_read_offset);
                        read_buffer_size -= next_read_offset;
                        next_read_offset = 0;
                    }
                    continue;
                case 1:
                    debug("handling_connection: resetting read buffer\n");
                    read_buffer_size = 0;
                    next_read_offset = 0;
                    break;
                default:
                    debug("handle_connection: unknown status %d from create_packet\n", status);
                    exit(EXIT_FAILURE);
            }

            debug("handle_connection: breaking buffer read loop\n");
            break;
        }
    }
}