#ifndef NET_PACKET_HANDLER_H
#define NET_PACKET_HANDLER_H 1

#include "net/packet/handshaking/packet_handshaking_handler.h"
#include "net/packet/status/packet_status_handler.h"
#include "net/packet/login/packet_login_handler.h"
#include "net/packet/play/packet_play_handler.h"
#include "net/connection.h"

void handle_packet(connection_t *conn, ReactorPacketPtr packet, byte_buffer_ptr buffer);

int send_packet(connection_t *conn, ReactorPacketPtr packet);

#endif