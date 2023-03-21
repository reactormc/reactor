#ifndef PACKET_PLAY_HANDLER_H
#define PACKET_PLAY_HANDLER_H 1

#include "../../connection.h"

void handle_play(ConnectionPtr conn, ReactorPacketPtr packet, byte_buffer_ptr buffer);

#endif