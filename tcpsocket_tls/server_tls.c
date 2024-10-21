#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>

typedef struct tls_socket {
    SSL* ssl;
    int fd;
} tls_socket;

int find_index_sock(tls_socket* tls_sockets, int sock, int top) {
    for (int i = 0; i <= top; i++) {
        if (tls_sockets[i].fd == sock) {
            return i;
        }
    }

    return -1;
}

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

SSL* create_server_ssl(SSL_CTX* ctx, tls_socket* tls_sockets, int client_sock) {
    tls_sockets->fd = client_sock;
    tls_sockets->ssl = SSL_new(ctx);
    SSL_set_accept_state(tls_sockets->ssl); // server: SSL_set_accept_state || client: SSL_set_connect_state
    SSL_set_fd(tls_sockets->ssl, client_sock);
    
    return tls_sockets->ssl;
}
