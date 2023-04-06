#ifndef NET_PACKET_STATUS_IN_STATUS_REQUEST_H
#define NET_PACKET_STATUS_IN_STATUS_REQUEST_H 1

#include "net/connection.h"

void handle_status_request(connection_t *conn, ReactorPacketPtr packet, byte_buffer_ptr buffer);

#endif
