#ifndef PACKET_READER_H
#define PACKET_READER_H 1

#include "packet/types.h"
#include "packet/handshaking/in/handshake.h"

int read_packet(char *buffer, int length, int *offset, int compressed, ReactorPacketPtr *packet);
void free_packet(ReactorPacketPtr packet);
PacketHandshakingInHandshake *read_packet_handshaking_in_handshake(ReactorPacketPtr packet, int *buffer_offset);

#endif
