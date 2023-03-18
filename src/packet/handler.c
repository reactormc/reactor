#include "handler.h"
#include "util/logger.h"
#include <sys/socket.h>

void handle_packet(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset) {
    debug("handle_packet: state: %d, packet id: %llu\n", conn->state, packet->packet_id);

    switch (conn->state) {
        case STATE_HANDSHAKING:
            handle_handshaking(conn, packet, read_buffer_offset);
            break;
        case STATE_STATUS:
            handle_status(conn, packet, read_buffer_offset);
            break;
        case STATE_LOGIN:
            handle_login(conn, packet, read_buffer_offset);
            break;
        case STATE_PLAY:
            handle_play(conn, packet, read_buffer_offset);
            break;
    }
}

int send_packet(ConnectionPtr conn, ReactorPacketPtr packet) {
    char *encoded = encode_packet(packet);
    if (!encoded) {
        return -1;
    }

    int total_size = (int) packet->packet_length + varint_encoding_length(packet->packet_length);

    debug("send_packet: sending packet, size %d\n", total_size);

    ssize_t total_bytes_sent = 0;

    while (1) {
        ssize_t bytes_sent = send(conn->remote_fd, encoded, total_size, 0);

        if (bytes_sent == -1) {
            perror("send_packet - send");
            return -1;
        } else {
            total_bytes_sent += bytes_sent;

            if (total_bytes_sent >= total_size) {
                break;
            }
        }
    }

    debug("send_packet: sent %d byte packet\n", total_bytes_sent);
    free_packet(packet);
    return 0;
}
