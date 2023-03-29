#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "connection.h"
#include "packet/packet_handler.h"
#include "../util/unicode_string.h"
#include "../util/logger.h"

static int set_profile_id(ConnectionPtr self, uint8_t *profile_id) {
    size_t len = u8_strlen(profile_id);

    if (!self->profile_id) {
        self->profile_id = calloc(len + 1, sizeof(uint8_t));
        if (!self->profile_id) {
            return -1;
        }
    }

    u8_strncpy(self->profile_id, profile_id, len);

    return 0;
}

static int set_username(ConnectionPtr self, uint8_t *username) {
    if (!self->username) {
        self->username = calloc(16, sizeof(ucs4_t));
        if (!self->username) {
            return -1;
        }
    }

    u8_strncpy(self->username, username, 16);
    return 0;
}

static int set_unique_id(ConnectionPtr self, uuid_t unique_id) {
    self->unique_id = unique_id;
    return 0;
}

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
        buffer->write(buffer, bytes_read, tmp);
    }

    return bytes_read;
}

/**
 * 1. not enough read to complete packet
 * 2. read just enough for packet
 * 3. read one packet and then some of another
 * @param remote_fd
 */
void handle_connection(server_t *server, int remote_fd) {
    printf("handle_connection: client connected\n");

    ConnectionPtr conn = calloc(1, sizeof(Connection));
    if (!conn) {
        perror("create_connection - calloc");
        exit(EXIT_FAILURE);
    }

    conn->remote_fd = remote_fd;
    conn->state = STATE_HANDSHAKING;
    conn->is_encrypted = 0;
    conn->set_profile_id = &set_profile_id;
    conn->set_username = &set_username;
    conn->set_unique_id = &set_unique_id;

    byte_buffer_ptr buffer;
    if (init_byte_buffer(&buffer) != BYTE_BUFFER_INIT_SUCCESS) {
        fprintf(stderr, "handle_connection: failed to initialize connection buffer\n");
        free(conn);
        exit(EXIT_FAILURE);
    }

    while (1) {
        debug("handle_connection: recv()\n");
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

        if (!buffer->has_more_bytes(buffer)) {
            debug("handle_connection: no more bytes to read\n");
            continue;
        }

        ReactorPacketPtr packet = NULL;
        switch (create_packet_from_header(buffer, 0, &packet)) {
            case -3:
                debug("handle_connection: skipping length zero packet\n");
                break;
            case -2:
                debug("handle_connection: read partial packet, re-rolling buffer\n");
                buffer->reroll(buffer);
                break;
            case -1:
                debug("handle_connection: failed to allocate space for packet\n");
                exit(EXIT_FAILURE);
            case 0:
                debug("handle_connection: handling packet\n");
                handle_packet(server, conn, packet, buffer);
                free_packet(packet);
                continue;
            default:
                debug("handle_connection: unknown status from create_packet_from_header\n");
                exit(EXIT_FAILURE);
        }
    }
}
