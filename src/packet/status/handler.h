#ifndef PACKET_STATUS_HANDLER_H
#define PACKET_STATUS_HANDLER_H 1

#include "connection.h"

void handle_status(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset);

#endif