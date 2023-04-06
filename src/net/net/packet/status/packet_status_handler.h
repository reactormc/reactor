#ifndef NET_PACKET_STATUS_HANDLER_H
#define NET_PACKET_STATUS_HANDLER_H 1

#include "net/connection.h"
#include "types.h"
#include "names.h"

void handle_status(connection_t *conn, ReactorPacketPtr packet, byte_buffer_ptr buffer);

#endif