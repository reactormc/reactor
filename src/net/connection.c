#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "connection.h"
#include "packet/packet_handler.h"
#include "../util/logger.h"

/**
 * 1. not enough read to complete packet
 * 2. read just enough for packet
 * 3. read one packet and then some of another
 * @param remote_fd
 */
void handle_connection(int remote_fd) {
    printf("handle_connection: client connected\n");

    ConnectionPtr conn = calloc(1, sizeof(Connection));
    if (!conn) {
        perror("create_connection - calloc");
        exit(EXIT_FAILURE);
    }

    conn->remote_fd = remote_fd;
    conn->state = STATE_HANDSHAKING;

    byte_buffer_ptr buffer;
    if (init_byte_buffer(&buffer) != BYTE_BUFFER_INIT_SUCCESS) {
        fprintf(stderr, "handle_connection: failed to initialize connection buffer\n");
        free(conn);
        exit(EXIT_FAILURE);
    }

    int do_read = 1;
    while (1) {
        if (do_read) {
            debug("handle_connection: reading packet(s) from network into buffer\n");

            int read_status = buffer->write_network(buffer, conn->remote_fd);
            if (read_status == BYTE_BUFFER_WRITE_FAILURE) {
                debug("handle_connection: network read failed\n");
                free(conn);
                free_byte_buffer(buffer);
                exit(EXIT_FAILURE);
            } else if (read_status == BYTE_BUFFER_WRITE_NETWORK_CLIENT_DISCONNECT) {
                printf("handle_connection: client disconnected\n");
                free(conn);
                free_byte_buffer(buffer);
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
            int status = create_packet_from_header(buffer, 0, &packet);

            switch (status) {
                case -2:
                    debug("handle_connection: could not create packet, nothing read from buffer; will reset buffer\n");
                    buffer->reroll(buffer);
                    do_read = 1;
                    break;
                case -1:
                    debug("handle_connection: failed to allocate space for packet\n");
                    exit(EXIT_FAILURE);
                case 0:
                    debug("handle_connection: handling packet\n");
                    handle_packet(conn, packet, buffer);
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
