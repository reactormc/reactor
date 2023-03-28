#ifndef PACKET_LOGIN_OUT_ENCRYPTION_REQUEST_H
#define PACKET_LOGIN_OUT_ENCRYPTION_REQUEST_H 1

#include "../../../connection.h"

void handle_login_plugin_response(ConnectionPtr conn, ReactorPacketPtr packet, byte_buffer_ptr buffer);

#endif