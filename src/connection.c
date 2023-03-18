#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "connection.h"
#include "util/logger.h"
#include "packet/handler.h"

int read_network(ConnectionPtr conn, char *buffer, int *buffer_size, const int *offset) {
    ssize_t bytes_read;

    if ((bytes_read = recv(conn->remote_fd, buffer + *offset, PACKET_BUFFER_SIZE - *offset, 0)) == -1) {
        perror("reactor: recv");
        return -1;
    }

    if (bytes_read == 0) {
        printf("reactor: client disconnected\n");
        return 1;
    }

    *buffer_size += (int) bytes_read;

    return 0;
}

/**
 * 1. not enough read to complete packet
 * 2. read just enough for packet
 * 3. read one packet and then some of another
 * @param remote_fd
 */
void handle_connection(int remote_fd) {
    printf("reactor: client connected\n");

    ConnectionPtr conn = calloc(1, sizeof(Connection));
    if (!conn) {
        perror("create_connection - calloc");
        exit(EXIT_FAILURE);
    }

    conn->remote_fd = remote_fd;
    conn->state = STATE_HANDSHAKING;

    char buffer[PACKET_BUFFER_SIZE] = {0}, tmp_buffer[PACKET_BUFFER_SIZE];

    int buffer_size = 0;
    int read_offset = 0;
    int do_read = 0;

    while (1) {
        if (do_read || buffer_size == 0) {
            debug("handle_connection: reading packet(s) from network into buffer\n");

            int read_status = read_network(conn, buffer, &buffer_size, &read_offset);
            if (read_status == -1) {
                free(conn);
                exit(EXIT_FAILURE);
            } else if (read_status == 1) {
                free(conn);
                exit(EXIT_SUCCESS);
            }

            debug("handle_connection: done reading from network\n");
            debug("handle_connection: toggling do_read off\n");
            do_read = 0;
            continue;
        }

        while (1) {
            debug("handle_connection: reading packet(s) from buffer\n");

            ReactorPacketPtr packet = NULL;

            int status = create_packet_from_header(buffer, PACKET_BUFFER_SIZE, &read_offset, 0, &packet);

            int bytes_remaining = PACKET_BUFFER_SIZE - read_offset;

            switch (status) {
                case -3:
                    debug("handle_connection: could not create packet, packet length already read; fixing buffer\n");
                    memcpy(tmp_buffer, buffer + read_offset, bytes_remaining);
                    memset(buffer, 0, PACKET_BUFFER_SIZE);
                    memcpy(buffer, tmp_buffer, bytes_remaining);
                    read_offset = bytes_remaining - 1;
                    buffer_size = bytes_remaining;
                    debug("handle_connection: toggling do_read on\n");
                    do_read = 1;
                    break;
                case -2:
                    debug("handle_connection: could not create packet, nothing read from buffer; will reset buffer\n");
                    read_offset = 0;
                    buffer_size = 0;
                    memset(buffer, 0, PACKET_BUFFER_SIZE);
                    debug("handle_connection: toggling do_read on\n");
                    do_read = 1;
                    break;
                case -1:
                    debug("handle_connection: failed to allocate space for packet\n");
                    exit(EXIT_FAILURE);
                case 0:
                    debug("handle_connection: handling packet\n");
                    handle_packet(conn, packet, &read_offset);
                    free_packet(packet);
                    continue;
                default:
                    debug("handle_connection: unknown status %d from create_packet\n", status);
                    exit(EXIT_FAILURE);
            }

            debug("handle_connection: breaking buffer read loop\n");
            break;
        }
    }
}
