#ifndef PACKET_PLAY_HANDLER_H
#define PACKET_PLAY_HANDLER_H 1

#include "connection.h"

void handle_play(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset);

#endif