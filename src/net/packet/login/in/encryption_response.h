#ifndef PACKET_LOGIN_IN_ENCRYPTION_RESPONSE_H
#define PACKET_LOGIN_IN_ENCRYPTION_RESPONSE_H 1

#include "../../../connection.h"

void handle_encryption_response(ConnectionPtr conn, ReactorPacketPtr packet, byte_buffer_ptr buffer);

#endif