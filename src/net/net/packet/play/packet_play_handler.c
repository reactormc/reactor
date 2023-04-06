#include "packet_play_handler.h"
#include "util/logger.h"

void handle_play(connection_t *conn, ReactorPacketPtr packet, byte_buffer_ptr buffer) {
    debug("got play packet %d\n", packet->packet_id);
}