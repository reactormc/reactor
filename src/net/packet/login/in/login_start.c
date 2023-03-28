#include "login_start.h"
#include "names.h"
#include "types.h"
#include "../out/disconnect.h"
#include "../out/encryption_request.h"
#include "../../../../util/logger.h"

#include <stdlib.h>

PacketLoginInLoginStart *read_login_start(ReactorPacketPtr parent, byte_buffer_ptr buffer) {
    PacketLoginInLoginStart *child = calloc(1, sizeof(PacketLoginInLoginStart));
    if (!child) {
        return NULL;
    }

    buffer->read_string(buffer, 16, &child->player_name);
    debug("read_login_start: read player_name %s\n", child->player_name);

    buffer->read_bool(buffer, &child->has_player_uuid);
    debug("read_login_start: read has_player_uuid %s\n", child->has_player_uuid ? "true" : "false");

    if (child->has_player_uuid) {
        buffer->read_uuid(buffer, &child->player_uuid);

//        char *uuid_str;
//        if (uuid_to_string(&child->player_uuid, &uuid_str) == 0) {
//            debug("read_login_start: read uuid %s\n", uuid_str);
//        }
    }

    return child;
}

void handle_login_start(ConnectionPtr conn, ReactorPacketPtr packet, byte_buffer_ptr buffer) {
    PacketLoginInLoginStart *login_start = read_login_start(packet, buffer);
    if (!login_start) {
        perror("handle_login_start");
        free_packet(packet);
        exit(EXIT_FAILURE);
    }

    char disconnect_message[1024];
    snprintf(disconnect_message, 1024, "{\"text\": \"Hello, \", \"extra\": [{\"text\": \"%s\", \"bold\": true, \"color\": \"#ff0000\"}, {\"text\": \"! You cannot join yet because I haven't coded that shit yet. Goodbye!\", \"bold\": false}]}", login_start->player_name);
    send_disconnect(conn, create_disconnect(disconnect_message));

    // TODO: send pk and vt...first generate those
    // send_encryption_request(conn, create_encryption_request());

    free(login_start);
}