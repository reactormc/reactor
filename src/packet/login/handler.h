#ifndef PACKET_LOGIN_HANDLER_H
#define PACKET_LOGIN_HANDLER_H 1

#include "connection.h"

void handle_login(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset);

#endif