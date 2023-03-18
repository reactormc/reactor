#include "writer.h"
#include "types.h"
#include "include/varint.h"
#include <stdio.h>
#include <unistr.h>

ReactorPacketPtr create_empty_packet(int packet_id) {
    ReactorPacketPtr ret = calloc(1, sizeof(ReactorPacket));
    if (!ret) {
        perror("create_empty_packet - calloc");
        return NULL;
    }

    ret->packet_length = varint_encoding_length(packet_id);
    ret->packet_id = packet_id;

    return ret;
}

void pack_packet_status_out_status_response(ReactorPacketPtr packet, PacketStatusOutStatusResponse *resp) {
    int length = u8_strlen(resp->response);

    packet->packet_length += length;
    packet->data = (char *) resp->response;
}

char *encode_packet(ReactorPacketPtr packet) {
    int buffer_len = packet->packet_length + varint_encoding_length(packet->packet_length);

    char *buffer = calloc(buffer_len, sizeof(char));
    if (!buffer) {
        perror("encode_packet - calloc");
        return NULL;
    }

    unsigned char bytes_tmp = 0;
    int bytes_written = 0;

    varint_encode(packet->packet_length, buffer, buffer_len, &bytes_tmp);
    bytes_written += bytes_tmp;

    varint_encode(packet->packet_id, buffer + bytes_written, buffer_len - bytes_written, &bytes_tmp);
    bytes_written += bytes_tmp;

    int remaining_length = packet->packet_length - bytes_tmp;
    for (int i = 0; i < remaining_length; i++, bytes_written++) {
        *(buffer + bytes_written) = *(packet->data + i);
    }

    return buffer;
}
