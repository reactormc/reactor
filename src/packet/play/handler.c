#include "handler.h"
#include "util/logger.h"

void handle_play(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset) {
    debug("got play packet %d\n", packet->packet_id);
}