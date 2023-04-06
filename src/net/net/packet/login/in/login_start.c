#include "login_start.h"
#include "net/packet/login/out/encryption_request.h"
#include "util/logger.h"

#include <stdlib.h>

PacketLoginInLoginStart *read_login_start(ReactorPacketPtr parent, byte_buffer_ptr buffer) {
    PacketLoginInLoginStart *child = calloc(1, sizeof(PacketLoginInLoginStart));
    if (!child) {
        return NULL;
    }

    // TODO: new player is added to list stored in server somehow?

    buffer->read_string(buffer, 16, &child->player_name);
    debug("read_login_start: read player_name %s\n", child->player_name);

    buffer->read_bool(buffer, &child->has_player_uuid);
    debug("read_login_start: read has_player_uuid %s\n", child->has_player_uuid ? "true" : "false");

    if (child->has_player_uuid) {
        buffer->read_uuid(buffer, &child->player_uuid);

        char *uuid_str;
        if (uuid_to_dashed_string(&child->player_uuid, &uuid_str) == 0) {
            debug("read_login_start: read uuid %s\n", uuid_str);
        }
    }

    return child;
}

void handle_login_start(connection_t *conn, ReactorPacketPtr packet, byte_buffer_ptr buffer) {
    debug("handle_login: got login start packet\n");

    PacketLoginInLoginStart *login_start = read_login_start(packet, buffer);
    if (!login_start) {
        perror("handle_login_start");
        free_packet(packet);
        exit(EXIT_FAILURE);
    }

    conn->set_username(conn, login_start->player_name);

    if (login_start->has_player_uuid == 1) {
        conn->set_unique_id(conn, login_start->player_uuid);
    }

    printf("reactor: received login start, sending encryption request\n");
    send_encryption_request(conn, create_encryption_request(conn->server));

    free(login_start);
}