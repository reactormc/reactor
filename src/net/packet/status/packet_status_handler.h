#ifndef PACKET_STATUS_HANDLER_H
#define PACKET_STATUS_HANDLER_H 1

#include "../../connection.h"
#include "types.h"
#include "names.h"

void handle_status(ConnectionPtr conn, ReactorPacketPtr packet, byte_buffer_ptr buffer);

#endif