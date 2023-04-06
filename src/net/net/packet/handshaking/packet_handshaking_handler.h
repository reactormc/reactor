#ifndef NET_PACKET_HANDSHAKING_HANDLER_H
#define NET_PACKET_HANDSHAKING_HANDLER_H 1

#include "net/connection.h"

void handle_handshaking(connection_t *conn, ReactorPacketPtr packet, byte_buffer_ptr buffer);

#endif