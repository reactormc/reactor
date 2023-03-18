#include <json-c/json_object.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <json-c/json.h>
#include "connection.h"
#include "include/varint.h"
#include "packet/reader.h"
#include "packet/types.h"
#include "packet/names.h"
#include "packet/writer.h"
#include "util/unicode_string.h"
#include "util/logger.h"

static int read_buffer_offset = -1;
static int read_buffer_size = 0;
static char read_buffer[PACKET_BUFFER_SIZE] = {0};

/* _read_network(ConnectionPtr, int): void {{{1 */
void _read_network(ConnectionPtr conn, int buffer_offset) {
    int bytes_read = 0;

    if ((bytes_read = recv(conn->remote_fd, read_buffer + buffer_offset, PACKET_BUFFER_SIZE - buffer_offset, 0))
        == -1) {

        perror("reactor: recv");
        exit(EXIT_FAILURE);
    }

    if (bytes_read == 0) {
        printf("reactor: client disconnected\n");
        close(conn->remote_fd);
        free(conn);
        exit(EXIT_SUCCESS);
    }

    read_buffer_size += bytes_read;
}
/* }}}1 */

/* create_connection(int): ConnectionPtr {{{1 */
ConnectionPtr create_connection(int remote_fd) {
    ConnectionPtr conn = calloc(1, sizeof(Connection));
    if (!conn) {
        perror("create_connection - calloc");
        return NULL;
    }

    conn->remote_fd = remote_fd;
    conn->state = STATE_HANDSHAKING;

    return conn;
}
/* }}}1 */

/* handle_connection(ConnectionPtr): void {{{1 */
void handle_connection(ConnectionPtr conn) {
    printf("reactor: client connected\n");

    if (!conn) {
        fprintf(stderr, "reactor: client was null upon handle_connection\n");
        exit(EXIT_FAILURE);
    }

    ReactorPacketPtr packet = NULL;
    int next_read_offset = 0;

    while (1) {
        if (read_buffer_offset >= 0) {
            int status;
            while (1) {
                if (read_buffer_offset + 1 >= read_buffer_size) {
                    debug("handle_connection: waiting for more data\n");
                    _read_network(conn, read_buffer_offset);
                } else {
                    debug("handle_connection: reading packet(s) from buffer\n");
                    if ((status = read_packet(read_buffer, PACKET_BUFFER_SIZE, &read_buffer_offset,
                                              0, &packet)) == 0) {

                        debug("handle_connection: got a packet, handling\n");
                        handle_packet(conn, packet, &read_buffer_offset);
                        free_packet(packet);
                        continue;
                    }

                    debug("handle_connection: bad return value, breaking loop\n");
                    break;
                }
            }

            if (status == -1) {
                debug("handle_connection: failed to allocate space for packet\n");
                exit(EXIT_FAILURE);
            }

            if (PACKET_BUFFER_SIZE - read_buffer_offset <= 0) {
                debug("handle_connection: we have used the entire buffer, resetting\n");
                memset(read_buffer, '0', PACKET_BUFFER_SIZE);
                next_read_offset = 0;
                read_buffer_size = 0;
            } else {
                debug("handle_connection: read_packet reported partial packet, preparing fresh space\n");
                char tmp_buf[PACKET_BUFFER_SIZE] = {0};
                int num_bytes = PACKET_BUFFER_SIZE - read_buffer_offset;
                memcpy(tmp_buf, read_buffer + read_buffer_offset, num_bytes);
                memset(read_buffer, '0', PACKET_BUFFER_SIZE);
                memcpy(read_buffer, tmp_buf, num_bytes);
                next_read_offset = num_bytes - 1;
                read_buffer_size = num_bytes;
            }
        }

        debug("handle_connection: reached bottom of connection loop, going back for more data\n");
        read_buffer_offset = 0;
        _read_network(conn, next_read_offset);
    }
}
/* }}}1 */

/* _handle_handshaking(ConnectionPtr, ReactorPacketPtr, int*): void {{{1 */
void _handle_handshaking(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset) {
    PacketHandshakingInHandshake *handshake = read_packet_handshaking_in_handshake(packet, read_buffer_offset);
    if (!handshake) {
        perror("_handle_handshaking");
        free_packet(packet);
        exit(EXIT_FAILURE);
    }

    debug("reactor: received handshake packet\n");

    if (handshake->next_state == STATE_STATUS) {
        debug("reactor: switching to status state\n");
        conn->state = STATE_STATUS;

        debug("reactor: creating json status response\n");
        /* json response building {{{2 */
        /* version */
        struct json_object *version = json_object_new_object();
        json_object_object_add(version, "name", json_object_new_string("1.19.4"));
        json_object_object_add(version, "protocol", json_object_new_int(762));
        /* end version */

        /* players */
        struct json_object *players = json_object_new_object();
        json_object_object_add(players, "max", json_object_new_int(420));
        json_object_object_add(players, "online", json_object_new_int(69));
        /* players->sample */
        struct json_object *sample = json_object_new_array_ext(2);
        struct json_object *firstPlayer = json_object_new_object();
        json_object_object_add(firstPlayer, "name", json_object_new_string("2008Choco"));
        json_object_object_add(firstPlayer, "id", json_object_new_string("73c62196-2af7-463d-8be1-a7a2270f4696"));
        struct json_object *secondPlayer = json_object_new_object();
        json_object_object_add(secondPlayer, "name", json_object_new_string("md_5"));
        json_object_object_add(secondPlayer, "id", json_object_new_string("af74a02d-19cb-445b-b07f-6866a861f783"));
        json_object_array_add(sample, firstPlayer);
        json_object_array_add(sample, secondPlayer);
        json_object_object_add(players, "sample", sample);
        /* end players->sample */
        /* end players */

        /* description */
        struct json_object *description = json_object_new_object();
        json_object_object_add(description, "text",
                               json_object_new_string("Welcome to the most 420/69 server in existence.\n"));
        /* end description */

        /* favicon */
        /* do not ask */
        struct json_object *favicon = json_object_new_string(
                "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAACXBIWXMAAAsTAAALEwEAmpwYAAAEwElEQVR4nO2ZYWwTZRjHn3Y4IQu6rcPeLcjsSsWx6cIGYoagRjAQxKkRERngN5e7dzV8wC8aQ2JEP/lBg8bERBP95AcS27lkRKMiG93o2OrGXVfB0bXbOmFsMtzo0D7mvdHm1vZa1pp4d7tf8mb9sCb3+z/PPe97VwADAwMDAwPdsubnyzZLj1gOSwpEs70reKy2d3R8TWcwCicFCywF1g9goaM79PnG/sjNrYNXsOJsEMElfgdLgUrP8Jv1/ZEZKk6XzRNEcIsIrcJu0DMPeIKNtb6xK3HxROWpvFscAEQT6BFr+1iR41zo+8f9fyTEF1ReWsIh0CO2rvDeul/HEu2uIO+Db7AA9ERF71BxlTf8U3LVU+VFBJfwDOgJW2dwT3y6Jy/ZPR+vfhvoCUd36NMGMbXqaSvvFqPw7YVq0EvLr/eO9KUTV5Cnrf8O6AHbL5d2yPf1O5KXBt9AIWgde9fwhw3CeExJPvWep5UXb4FLqANNg2he5w23KYkrV16q/rugZdZ6AvfUnA//nqO8D9oCd4NWuf9M0L7BNzaZm7w4Da2DVaBVKjsuP72pPxLNUR6h1d8EWqWyK9j8mDD+T87yLuET0Cpru4MfbRGVxbPKS1ve8ArQIuu8oVOZxKWtrjNj5aegLWAHtUAYfJu34rNZ/xHRXOUNd2eTz9L2MWgVXgS1wLN4jLCIPINRjsXGTK+rqs+P+POSnz/wvAVqkye3l1IIdI9/pG90JK+2n5f/EtTU9kQmLwvhpvx2oPK1vSOR/6Dyp1V12OEY3EkYnFUIQeoE1usvq+0bvZq/vHARXP4yUBtchhBet87NNThDM3nLu8Wr4L7gALXCpQmhmfkb6+7yYaWpA7e1hPKRj4JbfBLUDpcUQuO9EbTBGWnREDa/8FsOA0+IgUt8FbQClxTCrpWhRAh0bdoTWEzl6dA7ClqDk4VwsGwa7abOlBDuTF54D7QKx+DOg5brcw+aPQvk46tmQxZ5t/CFpn/NcbJY/1LJxKzd1JE2ACmEer9S25/U9I8ZnBU3Exan6C3wcukEZgqhemNKAG5Nv9B0slhPGLwm3w73lV5LmQOJ3QE60NYUiMufgh+HloOe5IlCCHaQfTZ1omP/xSFo7ysCPcqTpBAcprPYVHY9sUXWFfqw2Xorypfjc6BXeXJ77S2ZmD1g+XNOfliiJ8b4s4PmQnAuQp4ORjogsz1AaSYEZw7y8e9qPgRnHvKaDeGoBVe2MPjEvpLJ44ctN6bzkY9DX5rQlydpvhfjGWwGNXBkNZYSFj+jF0qnN53i0iGm4Bw2Fo/nLJ+hE2KERQJqgFuFDGFwkF6YXF6+niq6lLN8mhDUI0/hWfxBuifZGNYs61E80j6/sBOmiBUfhUVCrLhbNW1PISxulVf1tVUz+JC5K/2ZvsCbU+VVDWHwg+TWzhTCobIbf+lGnsKz+FW64ZYuBDobXimZfB/0BM/iCaUJT0OoKuhOyNMByTG4DfREC4sHMu3xNISHl/VI8nSLpOcE0BOHK3A5YXA4Uwh0d5j/iydAj5By3E5YTDy9KRxb/W9UYDHoFVKO23kGQwry7c770Ap658hqXEFY3M8z+DHP4NeEweOExS3/93UZGBgYGED+/AuD903B4vUcMQAAAABJRU5ErkJggg==");
        /* end favicon */

        /* enforcesSecureChat */
        struct json_object *enforcesSecureChat = json_object_new_boolean(0);
        /* end enforcesSecureChat */

        /* final message */
        struct json_object *response = json_object_new_object();
        json_object_object_add(response, "version", version);
        json_object_object_add(response, "players", players);
        json_object_object_add(response, "description", description);
        json_object_object_add(response, "favicon", favicon);
        json_object_object_add(response, "enforcesSecureChat", enforcesSecureChat);
        /* end final message */
        /* }}}2 */

        const char *data = json_object_to_json_string(response);
        char *out = encode_to_unicode_string(data, 32767); // max length of json response

        /* dispose object */
        json_object_put(response);

        debug("reactor: building response packet\n");
        PacketStatusOutStatusResponse resp;
        resp.response = (uint8_t *) out;

        ReactorPacketPtr packet_out = create_empty_packet(PACKET_STATUS_OUT_STATUS_RESPONSE);
        if (!packet_out) {
            free(out);
            free(handshake);
            fprintf(stderr, "Failed to create status response\n");
            return;
        }

        pack_packet_status_out_status_response(packet_out, &resp);

        char *encoded = encode_packet(packet_out);
        if (!encoded) {
            free(out);
            free(handshake);
            fprintf(stderr, "Failed to encode packet\n");
            return;
        }

        int total_size = packet_out->packet_length + varint_encoding_length(packet_out->packet_length);

        debug("reactor: writing response packet, size %d\n", total_size);
        int bytes_sent = 0;
        if ((bytes_sent = send(conn->remote_fd, encoded, total_size, 0)) == -1) {

            perror("_handle_handshaking - send");
        }

        debug("reactor: sent %d bytes\n", bytes_sent);
    } else if (handshake->next_state == STATE_LOGIN) {
        debug("reactor: switching to login state\n");
        conn->state = STATE_LOGIN;
    }

    free(handshake);
}
/* }}}1 */

/* _handle_status(ConnectionPtr, ReactorPacketPtr, int*): void {{{1 */
void _handle_status(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset) {
    debug("got status packet\n");
}
/* }}}1 */

/* _handle_login(ConnectionPtr, ReactorPacketPtr, int*): void {{{1 */
void _handle_login(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset) {
    debug("got login packet\n");
}
/* }}}1 */

/* _handle_play(ConnectionPtr, ReactorPacketPtr, int*): void {{{1 */
void _handle_play(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset) {
    debug("got play packet\n");
}
/* }}}1 */

/* handle_packet(int, ReactorPacketPtr*, int*): void {{{1 */
void handle_packet(ConnectionPtr conn, ReactorPacketPtr packet, int *read_buffer_offset) {
    switch (conn->state) {
        case STATE_HANDSHAKING:
            _handle_handshaking(conn, packet, read_buffer_offset);
            break;
        case STATE_STATUS:
            _handle_status(conn, packet, read_buffer_offset);
            break;
        case STATE_LOGIN:
            _handle_login(conn, packet, read_buffer_offset);
            break;
        case STATE_PLAY:
            _handle_play(conn, packet, read_buffer_offset);
            break;
    }
}
/* }}}1 */
