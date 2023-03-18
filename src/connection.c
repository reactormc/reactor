#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "connection.h"
#include "util/logger.h"
#include "packet/handler.h"

static int read_buffer_offset = -1;
static ssize_t read_buffer_size = 0;
static char read_buffer[PACKET_BUFFER_SIZE] = {0};

int read_network(ConnectionPtr conn, int buffer_offset) {
    ssize_t bytes_read;

    if ((bytes_read = recv(conn->remote_fd, read_buffer + buffer_offset,
                           PACKET_BUFFER_SIZE - buffer_offset, 0)) == -1) {
        perror("reactor: recv");
        return -1;
    }

    if (bytes_read == 0) {
        printf("reactor: client disconnected\n");
        return 1;
    }

    read_buffer_offset = buffer_offset;
    read_buffer_size += bytes_read;

    return 0;
}

void handle_connection(int remote_fd) {
    printf("reactor: client connected\n");

    ConnectionPtr conn = calloc(1, sizeof(Connection));
    if (!conn) {
        perror("create_connection - calloc");
        exit(EXIT_FAILURE);
        return;
    }

    conn->remote_fd = remote_fd;
    conn->state = STATE_HANDSHAKING;

    ReactorPacketPtr packet = NULL;
    int status, next_read_offset = 0;

    while (1) {
        if (read_buffer_offset <= 0 || read_buffer_offset + 1 >= read_buffer_size) {
            status = read_network(conn, next_read_offset);
            if (status == -1) {
                free(conn);
                exit(EXIT_FAILURE);
            } else if (status == 1) {
                free(conn);
                exit(EXIT_SUCCESS);
            }

            continue;
        }

        while (1) {
            debug("handle_connection: reading packet(s) from buffer\n");

            status = create_packet_from_header(read_buffer, PACKET_BUFFER_SIZE, &read_buffer_offset, 0, &packet);
            if (packet == NULL) {
                debug("handle_connection: null packet, skipping...");
                continue;
            }

            debug("handle_connection: got packet");
            switch (status) {
                case -2:
                    break;
                case -1:
                    debug("handle_connection: failed to allocate space for packet\n");
                    exit(EXIT_FAILURE);
                case 0:
                    debug("handle_connection: got a packet, handling\n");
                    handle_packet(conn, packet, &read_buffer_offset);
                    free_packet(packet);
                    continue;
                default:
                    debug("handle_connection: unknown status %d from create_packet\n", status);
                    exit(EXIT_FAILURE);
            }

            break;
        }

        if (PACKET_BUFFER_SIZE - read_buffer_offset <= 0) {
            debug("handle_connection: we have used the entire buffer, resetting\n");
            memset(read_buffer, '0', PACKET_BUFFER_SIZE);
            next_read_offset = 0;
            read_buffer_size = 0;
        } else {
            debug("handle_connection: read_packet reported partial packet, preparing fresh space\n");
            char tmp_buf[PACKET_BUFFER_SIZE] = {0};
            int num_bytes = PACKET_BUFFER_SIZE - read_buffer_offset;
            memcpy(tmp_buf, read_buffer + read_buffer_offset, num_bytes);
            memset(read_buffer, '0', PACKET_BUFFER_SIZE);
            memcpy(read_buffer, tmp_buf, num_bytes);
            next_read_offset = num_bytes - 1;
            read_buffer_size = num_bytes;
        }
    }
}