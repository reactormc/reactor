#ifndef NET_PACKET_HANDLER_H
#define NET_PACKET_HANDLER_H 1

#include "handshaking/packet_handshaking_handler.h"
#include "status/packet_status_handler.h"
#include "login/packet_login_handler.h"
#include "play/packet_play_handler.h"
#include "../connection.h"

void handle_packet(server_t *server, ConnectionPtr conn, ReactorPacketPtr packet, byte_buffer_ptr buffer);

int send_packet(ConnectionPtr conn, ReactorPacketPtr packet);

#endif