#ifndef NET_PACKET_LOGIN_OUT_ENCRYPTION_REQUEST_H
#define NET_PACKET_LOGIN_OUT_ENCRYPTION_REQUEST_H 1

#include "net/connection.h"

void handle_login_plugin_response(server_t *server, ConnectionPtr conn, ReactorPacketPtr packet, byte_buffer_ptr buffer);

#endif