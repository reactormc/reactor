#ifndef NET_PACKET_PLAY_HANDLER_H
#define NET_PACKET_PLAY_HANDLER_H 1

#include "../../connection.h"

void handle_play(server_t *server, ConnectionPtr conn, ReactorPacketPtr packet, byte_buffer_ptr buffer);

#endif