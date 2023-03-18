#ifndef PACKET_STATUS_IN_HANDLER_H
#define PACKET_STATUS_IN_HANDLER_H 1

#include "connection.h"
#include "types.h"
#include "names.h"

void handle_status_request(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset);

void handle_ping_request(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset);

#endif