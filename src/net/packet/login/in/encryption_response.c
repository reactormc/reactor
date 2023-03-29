#include "encryption_response.h"

#include "../out/set_compression.h"
#include "../../../../util/logger.h"
#include "../../../../util/unicode_string.h"
#include "../../../../../deps/sha1/sha1.h"

#include <tomcrypt.h>
#include <curl/curl.h>

PacketLoginInEncryptionResponse *read_encryption_response(ReactorPacketPtr parent, byte_buffer_ptr buffer) {
    PacketLoginInEncryptionResponse *child = calloc(1, sizeof(PacketLoginInEncryptionResponse));
    if (!child) {
        return NULL;
    }

    buffer->read_varint(buffer, &child->shared_secret_length);
    buffer->read_bytes(buffer, child->shared_secret_length, &child->shared_secret);
    buffer->read_varint(buffer, &child->verify_token_length);
    buffer->read_bytes(buffer, child->verify_token_length, &child->verify_token);

    return child;
}

static size_t session_write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    byte_buffer_ptr buffer = (byte_buffer_ptr) userdata;

    if (buffer->write_bytes(buffer, size * nmemb, (int8_t*) ptr) == BYTE_BUFFER_WRITE_SUCCESS) {
        return size * nmemb;
    }

    return 0;
}

void handle_encryption_response(server_t *server, ConnectionPtr conn, ReactorPacketPtr packet, byte_buffer_ptr buffer) {
    debug("handle_encryption_response: got encryption response packet\n");

    PacketLoginInEncryptionResponse *encryption_response = read_encryption_response(packet, buffer);
    if (!encryption_response) {
        // TODO: send disconnect
        perror("handle_encryption_response");
        free_packet(packet);
        exit(EXIT_FAILURE);
    }

    int8_t decrypted_shared_secret[encryption_response->shared_secret_length];

    int decrypt_status = 0;
    rsa_decrypt_key(
            (uint8_t *) encryption_response->shared_secret,
            encryption_response->shared_secret_length,
            (uint8_t *) decrypted_shared_secret,
            (unsigned long *) &encryption_response->shared_secret_length,
            NULL,
            0,
            0,
            &decrypt_status,
            &server->rsa_key
    );

    if (decrypt_status == 0) {
        // TODO: send disconnect
        fprintf(stderr, "handle_encryption_response: failed to decrypt shared secret\n");
        free_packet(packet);
        exit(EXIT_FAILURE);
    }

    conn->shared_secret = calloc(encryption_response->shared_secret_length, sizeof(int8_t));
    if (!conn->shared_secret) {
        // TODO: send disconnect
        fprintf(stderr, "handle_encryption_response: failed to allocate space for shared secret\n");
        free_packet(packet);
        exit(EXIT_FAILURE);
    }

    memcpy(conn->shared_secret, encryption_response->shared_secret, encryption_response->shared_secret_length);

    SHA1_CTX *sha1_ctx = NULL;
    SHA1Init(sha1_ctx);
    if (!sha1_ctx) {
        // TODO: send disconnect
        fprintf(stderr, "handle_encryption_response: failed to initialize sha1\n");
        free_packet(packet);
        free(conn->shared_secret);
        exit(EXIT_FAILURE);
    }

    SHA1Update(sha1_ctx, server->server_id, u8_strlen(server->server_id));
    SHA1Update(sha1_ctx, (uint8_t*) decrypted_shared_secret, encryption_response->shared_secret_length);
    SHA1Update(sha1_ctx, (uint8_t*) server->public_key_x509, server->public_key_x509_len);

    uint8_t *sha1_digest = NULL;
    SHA1Final(sha1_digest, sha1_ctx);

    debug("reactor: generated login hash %s\n", sha1_digest);

    CURL *curl = curl_easy_init();
    if (!curl) {
        // TODO: send disconnect
        fprintf(stderr, "handle_encryption_response: failed to initialize cURL\n");
        free_packet(packet);
        free(conn->shared_secret);
        exit(EXIT_FAILURE);
    }

    uint8_t *session_url = calloc(256, sizeof(uint8_t));
    if (!session_url) {
        // TODO: send disconnect
        fprintf(stderr, "handle_encryption_response: failed to create session URL\n");
        free_packet(packet);
        free(conn->shared_secret);
        exit(EXIT_FAILURE);
    }

    snprintf((char*) session_url, 256, "https://sessionserver.mojang.com/session/minecraft/hasJoined?username=%s&serverId=%s", conn->username, sha1_digest);

    byte_buffer_ptr session_resp_buffer;
    if (init_byte_buffer(&session_resp_buffer) != BYTE_BUFFER_INIT_SUCCESS) {
        // TODO: send disconnect
        fprintf(stderr, "handle_encryption_response: failed to initialize buffer for session response\n");
        exit(EXIT_FAILURE);
    }

    CURLcode res;
    curl_easy_setopt(curl, CURLOPT_URL, session_url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &session_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, session_resp_buffer);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        // TODO: send disconnect
        fprintf(stderr, "handle_encryption_response: failed to send request to mojang session server (is it down?)\n");
        exit(EXIT_FAILURE);
    }

    int64_t response_code;
    if (curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code) != CURLE_OK) {
        // TODO: send disconnect
        fprintf(stderr, "handle_encryption_response: failed to extract response code\n");
        exit(EXIT_FAILURE);
    }

    if (response_code != 200) {
        // TODO: send disconnect
        fprintf(stderr, "handle_encryption_response: unexpected response from session server\n");
        exit(EXIT_FAILURE);
    }

    // TODO: set profile id & name & maybe skin blob?

    curl_easy_cleanup(curl);
    free_byte_buffer(session_resp_buffer);
    free(session_url);
    free_packet(packet);
}