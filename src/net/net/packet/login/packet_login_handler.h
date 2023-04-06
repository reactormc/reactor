#ifndef NET_PACKET_LOGIN_HANDLER_H
#define NET_PACKET_LOGIN_HANDLER_H 1

#include "net/connection.h"
#include "types.h"
#include "names.h"

void handle_login(connection_t *conn, ReactorPacketPtr packet, byte_buffer_ptr buffer);

#endif