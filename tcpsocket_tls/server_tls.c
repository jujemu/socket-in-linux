#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>

typedef struct tls_socket {
    SSL* ssl;
    int fd;
} tls_socket;

SSL_CTX* create_server_ssl_ctx(char* certificate_path, char* key_path) {
    const SSL_METHOD* method = TLS_server_method();
    SSL_CTX* ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Creating SSL context fails.");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    /* load certificate of server */
    if (SSL_CTX_use_certificate_file(ctx, certificate_path, SSL_FILETYPE_PEM) <= 0 ||
    SSL_CTX_use_PrivateKey_file(ctx, key_path, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

void create_server_ssl(SSL_CTX* ctx, tls_socket* tls_sockets, int client_sock) {
    tls_sockets->fd = client_sock;
    tls_sockets->ssl = SSL_new(ctx);
    SSL_set_accept_state(tls_sockets->ssl); // server: SSL_set_accept_state || client: SSL_set_connect_state
    SSL_set_fd(tls_sockets->ssl, client_sock);
}
