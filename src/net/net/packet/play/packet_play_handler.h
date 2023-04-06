#ifndef NET_PACKET_PLAY_HANDLER_H
#define NET_PACKET_PLAY_HANDLER_H 1

#include "net/connection.h"

void handle_play(connection_t *conn, ReactorPacketPtr packet, byte_buffer_ptr buffer);

#endif