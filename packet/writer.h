#include "types.h"
#ifndef WRITER_H
#define WRITER_H 1

ReactorPacketPtr create_empty_packet(int packet_id);
void pack_packet_status_out_status_response(ReactorPacketPtr packet, PacketStatusOutStatusResponse *resp);
char *encode_packet(ReactorPacketPtr packet);

#endif
