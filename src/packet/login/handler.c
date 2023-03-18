#include "handler.h"
#include "util/logger.h"

void handle_login(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset) {
    debug("got login packet %d\n", packet->packet_id);
}