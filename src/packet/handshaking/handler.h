#ifndef PACKET_HANDSHAKING_HANDLER_H
#define PACKET_HANDSHAKING_HANDLER_H 1

#include "connection.h"

void handle_handshaking(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset);

#endif