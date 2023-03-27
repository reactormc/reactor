#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "connection.h"
#include "packet/packet_handler.h"
#include "../util/logger.h"

/**
 * Receives data from the given socket and writes it into the given byte buffer.
 *
 * @param remote_fd the socket
 * @param buffer the byte buffer
 * @return the number of bytes read
 */
static int poll_network(int remote_fd, byte_buffer_ptr buffer) {
    char tmp[INITIAL_BYTE_BUFFER_SIZE] = {0};

    int bytes_read;
    if ((bytes_read = (int) recv(remote_fd, tmp, INITIAL_BYTE_BUFFER_SIZE, 0)) == -1) {
        return -1;
    }

    if (bytes_read > 0) {
        buffer->write(buffer, tmp, bytes_read);
    }

    return bytes_read;
}

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

    int rerolled = 0;

    while (1) {
        if (rerolled || buffer->is_empty(buffer)) {
            debug("handle_connection: reading network\n");
            int bytes_read = poll_network(remote_fd, buffer);
            if (bytes_read == -1) {
                debug("handle_connection: network read fail\n");
                free_byte_buffer(buffer);
                free(conn);
                exit(EXIT_FAILURE);
            } else if (bytes_read == 0) {
                debug("handle_connection: client disconnected\n");
                free_byte_buffer(buffer);
                free(conn);
                exit(EXIT_SUCCESS);
            }

            rerolled = 0;
        }

        ReactorPacketPtr packet = NULL;
        switch (create_packet_from_header(buffer, 0, &packet)) {
            case -3:
                debug("handle_connection: nothing left in buffer\n");
                buffer->reset(buffer);
                break;
            case -2:
                debug("handle_connection: read partial packet, re-rolling buffer\n");
                buffer->reroll(buffer);
                rerolled = 1;
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
                debug("handle_connection: unknown status from create_packet_from_header\n");
                exit(EXIT_FAILURE);
        }
    }
}
