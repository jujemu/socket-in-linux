#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#include "error_handler.h"

#define BUF_SIZE 256

enum thread_error_code {
    SERVER_CLOSED = -1,
    FAIL_TO_READ = -2
};

/* Library manage the core key of crypto, and give user a context that can access the core key. */
SSL_CTX* create_client_ssl_ctx(char* ca_cert_path) {
    const SSL_METHOD* method = TLS_method();
    SSL_CTX* ctx = SSL_CTX_new(method);
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        error_handle("Error occurs when creating SSL context.");
    }

    /* client can verify certifate of server with CA certificate. */
    SSL_CTX_set_options(ctx, SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3); // Recommended to avoid SSLv2 & SSLv3
    SSL_CTX_set_verify_depth(ctx, 1);
    if (!SSL_CTX_load_verify_locations(ctx, ca_cert_path, NULL)) {
        ERR_print_errors_fp(stderr);
        error_handle("Fail to load certificate.");
    }

    return ctx;
}

SSL* create_client_ssl(SSL_CTX* ctx, int sock) {
    SSL* ssl = SSL_new(ctx);
    
    SSL_set_connect_state(ssl); // server: SSL_set_accept_state || client: SSL_set_connect_state
    SSL_set_fd(ssl, sock); // wrap the socket with SSL
    
    return ssl;
}

/* print ssl status before and after TLS handshaking. */
void do_and_print_status_ssl_handshake(SSL* ssl) {
    const char* prev_state = { 0, };
    const char* current_state = { 0, };    
    
    prev_state = SSL_state_string_long(ssl);
    printf("SSL-STATE: %s\n", prev_state);

    SSL_do_handshake(ssl);

    current_state = SSL_state_string_long(ssl);
    if (current_state != prev_state) {
        printf("SSL-STATE: %s\n\n", current_state);
    } else {
        show_last_error_msg();
        error_handle("SSL handshake fails.");
    }
}

int do_ssl_handshake(SSL* ssl) {
    char init_msg[BUF_SIZE] = { "Succcessfully connected with client" };

    do_and_print_status_ssl_handshake(ssl);

    if (SSL_accept(ssl) != 1) {
        show_last_error_msg();
        error_handle("Fail to accept connection of SSL.");
    }
    
    /* send init message to server */
    SSL_write(ssl, init_msg, strlen(init_msg)+1);

    return 0;
}

/* Execute reading socket in new thread. */
void *read_sock_t(void* param) {
    SSL* ssl = (SSL*) param;
    char stdin_read_buf[BUF_SIZE] = { 0, };
    ssize_t bytes_received = 0;
    
    while (1) {
        bytes_received = SSL_read(ssl, stdin_read_buf, BUF_SIZE);
        
        if (bytes_received == 0) {
            return (void*) SERVER_CLOSED;
        }

        if (bytes_received < 0) {
            return (void*) FAIL_TO_READ;
        }

        printf("%s\n", stdin_read_buf);
    }
    
    return NULL;
}
