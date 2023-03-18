#ifndef PACKET_STATUS_IN_STATUS_REQUEST_H
#define PACKET_STATUS_IN_STATUS_REQUEST_H 1

#include "connection.h"

void handle_status_request(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset);

#endif
