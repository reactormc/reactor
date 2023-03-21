#ifndef PACKET_LOGIN_HANDLER_H
#define PACKET_LOGIN_HANDLER_H 1

#include "../../connection.h"

void handle_login(ConnectionPtr conn, ReactorPacketPtr packet, byte_buffer_ptr buffer);

#endif