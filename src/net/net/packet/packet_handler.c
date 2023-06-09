#include "packet_handler.h"
#include "util/logger.h"
#include <sys/socket.h>

void handle_packet(connection_t *conn, ReactorPacketPtr packet, byte_buffer_ptr buffer) {
    debug("handle_packet: state: %d, packet id: %llu\n", conn->state, packet->packet_id);

    switch (conn->state) {
        case STATE_HANDSHAKING:
            handle_handshaking(conn, packet, buffer);
            break;
        case STATE_STATUS:
            handle_status(conn, packet, buffer);
            break;
        case STATE_LOGIN:
            handle_login(conn, packet, buffer);
            break;
        case STATE_PLAY:
            handle_play(conn, packet, buffer);
            break;
    }
}

int send_packet(ConnectionPtr conn, ReactorPacketPtr packet) {
    byte_buffer_ptr encoded = encode_packet(packet);
    if (!encoded) {
        return -1;
    }

    int total_size = (int) packet->packet_length + varint_encoding_length(packet->packet_length);

    debug("send_packet: sending packet, size %d\n", total_size);

    int total_bytes_sent = 0;

    while (1) {
        debug("send_packet: calling send()\n");
        ssize_t bytes_sent = send(conn->remote_fd, encoded->bytes_at(encoded, total_bytes_sent), total_size - total_bytes_sent, 0);

        if (bytes_sent == -1) {
            perror("send_packet: send");
            return -1;
        } else {
            debug("send_packet: sent %d bytes, recording...\n", bytes_sent);

            total_bytes_sent += (int) bytes_sent;

            if (total_bytes_sent >= total_size) {
                debug("send_packet: no more tries required\n");
                break;
            }
        }
    }

    debug("send_packet: sent %d byte packet\n", total_bytes_sent);
    free_byte_buffer(encoded);
    free_packet(packet);
    return 0;
}
