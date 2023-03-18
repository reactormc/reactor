#include "handler.h"
#include "util/logger.h"

void handle_status(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset) {
    debug("got status packet %d\n", packet->packet_id);
}