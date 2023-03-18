#ifndef PACKET_HANDLER_H
#define PACKET_HANDLER_H 1

#include "handshaking/handler.h"
#include "status/handler.h"
#include "login/handler.h"
#include "play/handler.h"

void handle_packet(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset);
int send_packet(ConnectionPtr conn, ReactorPacketPtr packet);

#endif