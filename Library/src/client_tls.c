#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#include "client_tls.h"
#include "config.h"
#include "error_handler.h"

void ssl_init() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ERR_load_crypto_strings();
}

/* Library manage the core key of crypto, and give user context that can access the core key. */
SSL_CTX* create_ssl_ctx() {
    const SSL_METHOD* method = TLS_method();
    SSL_CTX* ctx = SSL_CTX_new(method);
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        error_handle("Error occurs when creating SSL context.");
    }

    return ctx;
}

/* client can verify certifate of server with CA certificate. */
void ssl_ctx_config(SSL_CTX* ctx, char* ca_cert_path) {
    //Recommended to avoid SSLv2 & SSLv3
    SSL_CTX_set_options(ctx, SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);
    SSL_CTX_set_verify_depth(ctx, 1);
    if (!SSL_CTX_load_verify_locations(ctx, ca_cert_path, NULL)) {
        ERR_print_errors_fp(stderr);
        error_handle("Fail to load certificate.");
    }
}

SSL* create_ssl(SSL_CTX* ctx, int sock) {
    SSL* ssl = SSL_new(ctx);
    printf("ssl 생성 직후 포인터 위치: %p\n", ssl);
    
    SSL_set_connect_state(ssl); // server: SSL_set_accept_state || client: SSL_set_connect_state
    SSL_set_fd(ssl, sock); // wrap the socket with SSL
    // SSL_set_bio(ssl, BIO_new(BIO_s_mem()), BIO_new(BIO_s_mem()));
    return ssl;
}

int do_ssl_handshake(SSL* ssl) {
    char msg[BUF_SIZE] = { "Succcessfully connected with client" };
    const char* prev_state = { 0, };
    const char* current_state = { 0, };

    /* print ssl status before and after TLS handshaking. */
    prev_state = SSL_state_string_long(ssl);
    printf("SSL-STATE: %s\n", prev_state);
    int handshake_rtn = SSL_do_handshake(ssl);
    current_state = SSL_state_string_long(ssl);
    if (current_state != prev_state) {
        printf("SSL-STATE: %s\n\n", current_state);
    }

    if (SSL_accept(ssl) != 1) {
        error_handle("잘못된 만남");
    }
    
    /* send init message to server */
    SSL_write(ssl, msg, strlen(msg)+1);

    return 0;
}

/* Execution of reading socket in new thread. */
void *read_sock_t(void* param) {
    SSL* ssl = (SSL*) param;
    char stdin_read_buf[BUF_SIZE] = { 0, };
    ssize_t bytes_received = 0;
    char buf[BUF_SIZE] = { 0, };
    
    while (1) {
        bytes_received = SSL_read(ssl, stdin_read_buf, BUF_SIZE);
        
        if (bytes_received == 0) {
            error_handle("Server connection closed.");
        }

        if (bytes_received < 0) {
            unsigned long err_num = ERR_get_error();
            printf("err_num : %ld\n", err_num);
            ERR_error_string(err_num, buf);
            printf("err: %s\n", buf);
            // printf("asdfdd\n");
            error_handle("Error occurs when reading socket.");
        }

        printf("%s\n", stdin_read_buf);
    }
    
    return NULL;
}
