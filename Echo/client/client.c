#define __USE_GNU // needed to include pthread_tryjoin_np
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
        show_last_error_msg();
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

/* if thread suddenly died, check the status of thread return. */
void check_thread_status(pthread_t* thread_1, void* thread_rtn) {
    pthread_tryjoin_np(*thread_1, &thread_rtn); // non-blocking pthread_join
    if (thread_rtn) {
        if (SERVER_CLOSED == *(enum thread_error_code*)thread_rtn) {
            error_handle("Server connection closed.");
        } else if (FAIL_TO_READ == *(enum thread_error_code*)thread_rtn) {
            show_last_error_msg();
            error_handle("Error occurs when reading socket.");
        }
    }
}

int main(void) {
    int client_sock = 0;
    SSL_CTX* ctx = NULL;
    SSL* ssl = NULL;
    char stdin_read_buf[BUF_SIZE] = { 0, };
    char sock_read_buf[BUF_SIZE] = { 0, };
    void* thread_rtn = NULL;

    int port = 443;
    char ip_addr[BUF_SIZE] = { "localhost" };
    char ca_cert_path[BUF_SIZE] = { "certificate/rootca.crt" };

    client_sock = create_tcp_sock();
    connect_with_serv(client_sock, port, ip_addr);

    /* ssl_context config and create ssl */
    ssl_init();
    ctx = create_ssl_ctx();
    ssl_ctx_config(ctx, ca_cert_path);
    ssl = create_ssl(ctx, client_sock);

    do_ssl_handshake(ssl);

    /* create thread for reading socket */
    pthread_t thread_1;
    if (pthread_create(&thread_1, NULL, read_sock_t, (void*)ssl) != 0) {
        close(client_sock);
        error_handle("Failed to create thread.");
    }

    /* send message to server */
    while (1) {
        if (send_to_serv(ssl, stdin_read_buf) != 0) {
            break;
        }

        check_thread_status(&thread_1, thread_rtn);
    }

    close(client_sock);

    return 0;
}
