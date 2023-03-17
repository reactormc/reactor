#include "reader.h"
#include "types.h"
#include "../include/varint.h"
#include "../util/unicode_string.h"
#include "../util/byte_utils.h"
#include "../util/logger.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* varint_decode_offset(char*, int, int*): VarInt {{{1 */
VarInt varint_decode_offset(char *buffer, int length, int *offset) {
    unsigned char bytes_read = 0;

    unsigned long long varint = varint_decode(buffer + *offset, length - *offset, &bytes_read);

    *offset += bytes_read;

    return varint;
}
/* }}}1 */

/* _read_uint16(char*, int, int*): uint16_t {{{1 */
uint16_t _read_uint16(char* bytes, int *offset) {
    uint8_to_uint16 converter;

    converter.chars[0] = *(bytes + *offset + 1);
    converter.chars[1] = *(bytes + *offset);
    
    *offset += 2;

    return converter.short_value;
}
/* }}}1 */

/* read_packet(char*, int, int): ReactorPacketPtr {{{1 */
int read_packet(char *buffer, int length, int *offset, int compressed, ReactorPacketPtr *packet) {
    *packet = calloc(1, sizeof(ReactorPacket));
    if (!*packet) {
        perror("read_packet: calloc");
        return -1;
    }

    if (*offset < 0 || *offset >= length) {
        debug("Offset out of bounds\n");
        return -2;
    }

    int remaining_length = length - *offset;
    
    if (length - *offset < VARINT_MAX_LEN) {
        debug("Cannot read a full varint\n");
        return -2;
    }

    int packet_length = varint_decode_offset(buffer, length, offset); 

    if (packet_length > remaining_length) {
        debug("Cannot read entire packet\n");
        return -2;
    }

    if (compressed) {
        (*packet)->data_length = varint_decode_offset(buffer, length, offset);
    }

    int offset_before = *offset;
    (*packet)->packet_id = varint_decode_offset(buffer, length, offset);

    // Packet length updated to be the length of remaining buffer data
    packet_length -= (*offset - offset_before); 
    (*packet)->packet_length = packet_length; 

    char* data_buf = calloc(packet_length, sizeof(char));
    if (!data_buf) {
        perror("read_packet: calloc");
        return -1;
    }

    strncpy(data_buf, buffer + *offset, packet_length);
    (*packet)->data = data_buf;
    *offset += packet_length;

    return 0;
}
/* }}}1 */

/* free_packet(ReactorPacketPtr): void {{{1 */
void free_packet(ReactorPacketPtr packet) {
    if (!packet) {
        return;
    }

    if (packet->data) {
        free(packet->data);
    }

    free(packet);
}
/* }}}1 */

/* read_packet_handshaking_in_handshake(ReactorPacketPtr*): PacketHandshakingInHandshake {{{1 */
PacketHandshakingInHandshake *read_packet_handshaking_in_handshake(ReactorPacketPtr parent, int *buffer_offset) {
    PacketHandshakingInHandshake *child = calloc(1, sizeof(PacketHandshakingInHandshake));
    if (!child) {
        return NULL;
    }

    int offset = 0;
    
    child->protocol_version = varint_decode_offset(parent->data, parent->packet_length, &offset);
    child->server_address = read_unicode_string(parent->data, parent->packet_length - offset, &offset);
    child->server_port = _read_uint16(parent->data, &offset);
    child->next_state = varint_decode_offset(parent->data, parent->packet_length - offset, &offset);

    *buffer_offset += offset;

    return child;
}
/* }}}1 */
