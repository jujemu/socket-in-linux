#include <openssl/ssl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "client_socket.h"
#include "client_tls.h"
#include "config.h"
#include "error_handler.h"

int send_to_serv(SSL* ssl, char* send_msg_buf) {
    memset(send_msg_buf, 0, BUF_SIZE);
    if (fgets(send_msg_buf, BUF_SIZE, stdin) == NULL) {
        error_handle("Error occurs when reading stdin buffer.\n\n");
    }

    /* remove new line character */
    if (send_msg_buf[0] == 0) { // check if char array is empty
        error_handle("Message buffer for writing is empty\n\n");
    }
    size_t length = strlen(send_msg_buf) - 1;
    if (*send_msg_buf && send_msg_buf[length] == '\n') {
        send_msg_buf[length] = '\0';
    }

    SSL_write(ssl, send_msg_buf, BUF_SIZE);
    if (strcmp(send_msg_buf, "!q") == 0) {
        return -1;
    }

    return 0;
}

int main(void) {
    int client_sock = 0;
    SSL_CTX* ctx = NULL;
    SSL* ssl = NULL;
    char stdin_read_buf[BUF_SIZE] = { 0, };
    char sock_read_buf[BUF_SIZE] = { 0, };

    int port = 443;
    char ip_addr[BUF_SIZE] = { "localhost" };
    char ca_cert_path[BUF_SIZE] = { "certificate/rootca.crt" };

    client_sock = create_tcp_sock();
    connect_with_serv(client_sock, port, ip_addr);

    // ssl_context config and create ssl
    ssl_init();
    ctx = create_ssl_ctx();
    ssl_ctx_config(ctx, ca_cert_path);
    ssl = create_ssl(ctx, client_sock);

    do_ssl_handshake(ssl);

    // create thread for reading socket
    pthread_t thread_1;
    if (pthread_create(&thread_1, NULL, read_sock_t, (void*)ssl) != 0) {
        close(client_sock);
        error_handle("Failed to create thread.");
    }

    // send message to server
    while (1) {
        if (send_to_serv(ssl, stdin_read_buf) != 0) {
            break;
        }
    }

    close(client_sock);

    return 0;
}
