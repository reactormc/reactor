#include "packet_login_handler.h"
#include "../../../util/logger.h"

void handle_login(ConnectionPtr conn, ReactorPacketPtr packet, byte_buffer_ptr buffer) {
    debug("got login packet %d\n", packet->packet_id);
}