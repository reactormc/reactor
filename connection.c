#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "connection.h"
#include "packet/reader.h"
#include "packet/types.h"
#include "util/logger.h"

static int read_buffer_offset = -1;
static int read_buffer_size = 0;
static char read_buffer[PACKET_BUFFER_SIZE] = {0};

/* _read_network(ConnectionPtr, int): void {{{1 */
void _read_network(ConnectionPtr conn, int buffer_offset) {
    int bytes_read = 0;

    if ((bytes_read = recv(conn->remote_fd, read_buffer + buffer_offset, PACKET_BUFFER_SIZE - buffer_offset, 0)) 
            == -1) {

        perror("reactor: recv");
        exit(EXIT_FAILURE);
    }

    if (bytes_read == 0) {
        printf("reactor: client disconnected\n");
        close(conn->remote_fd);
        free(conn);
        exit(EXIT_SUCCESS);
    }

    read_buffer_size += bytes_read;
}
/* }}}1 */

/* create_connection(int): ConnectionPtr {{{1 */
ConnectionPtr create_connection(int remote_fd) {
    ConnectionPtr conn = calloc(1, sizeof(Connection));
    if (!conn) {
        perror("create_connection - calloc");
        return NULL;
    }

    conn->remote_fd = remote_fd;
    conn->state = STATE_HANDSHAKING;

    return conn;
}
/* }}}1 */

/* handle_connection(ConnectionPtr): void {{{1 */
void handle_connection(ConnectionPtr conn) {
    printf("reactor: client connected\n");

    if (!conn) {
        fprintf(stderr, "reactor: client was null upon handle_connection\n");
        exit(EXIT_FAILURE);
    }

    ReactorPacketPtr packet = NULL;
    int next_read_offset = 0;
    
    while (1) {
        if (read_buffer_offset >= 0) {
            int status;
            while (1) {
                if (read_buffer_offset + 1 >= read_buffer_size) {            
                    debug("handle_connection: waiting for more data\n");
                    _read_network(conn, read_buffer_offset);
                } else {
                    debug("handle_connection: reading packet(s) from buffer\n");
                    if ((status = read_packet(read_buffer, PACKET_BUFFER_SIZE, &read_buffer_offset, 
                                    0, &packet)) == 0) {

                        debug("handle_connection: got a packet, handling\n");
                        handle_packet(conn, packet, &read_buffer_offset);
                        free_packet(packet);
                        continue;
                    }

                    debug("handle_connection: bad return value, breaking loop\n");
                    break;
                }
            }

            if (status == -1) {
                debug("handle_connection: failed to allocate space for packet\n");
                exit(EXIT_FAILURE);
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

        debug("handle_connection: reached bottom of connection loop, going back for more data\n");
        read_buffer_offset = 0;
        _read_network(conn, next_read_offset);
    }
}
/* }}}1 */

/* _handle_handshaking(ConnectionPtr, ReactorPacketPtr, int*): void {{{1 */
void _handle_handshaking(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset) {
    PacketHandshakingInHandshake *handshake = read_packet_handshaking_in_handshake(packet, read_buffer_offset);
    if (!handshake) {
        perror("_handle_handshaking");
        free_packet(packet);
        exit(EXIT_FAILURE);
    }

    debug("got handshake packet!\n");
    debug("proto version: %llu\n", handshake->protocol_version);
    debug("server address: %s\n", handshake->server_address);
    debug("server port: %d\n", handshake->server_port);
    debug("next state: %llu\n", handshake->next_state);

    free(handshake);
}
/* }}}1 */

/* _handle_status(ConnectionPtr, ReactorPacketPtr, int*): void {{{1 */
void _handle_status(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset) {
    free_packet(packet);
}
/* }}}1 */

/* _handle_login(ConnectionPtr, ReactorPacketPtr, int*): void {{{1 */
void _handle_login(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset) {
    free_packet(packet);
}
/* }}}1 */

/* _handle_play(ConnectionPtr, ReactorPacketPtr, int*): void {{{1 */
void _handle_play(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset) {
    free_packet(packet);
}
/* }}}1 */

/* handle_packet(int, ReactorPacketPtr*, int*): void {{{1 */
void handle_packet(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset) {
    switch(conn->state) {
        case STATE_HANDSHAKING:
            _handle_handshaking(conn, packet, read_buffer_offset);
            break;
        case STATE_STATUS:
            _handle_status(conn, packet, read_buffer_offset);
            break;
        case STATE_LOGIN:
            _handle_login(conn, packet, read_buffer_offset);
            break;
        case STATE_PLAY:
            _handle_play(conn, packet, read_buffer_offset);
            break;
    }
}
/* }}}1 */
