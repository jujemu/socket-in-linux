#include "server_tls.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>

SSL_CTX* ctx;

void ssl_init() 
{
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();

    // create SSL Context
    const SSL_METHOD* method = TLS_server_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) 
    {
        perror("Creating SSL context fails.");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // load certificate of server
    if (SSL_CTX_use_certificate_file(ctx, CERTIFICATE_PATH, SSL_FILETYPE_PEM) <= 0 ||
    SSL_CTX_use_PrivateKey_file(ctx, KEY_PATH, SSL_FILETYPE_PEM) <= 0) 
    {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

int find_index_sock(ssl_client* clients, int sock, int top) 
{
    for (int i = 0; i <= top; i++) 
    {
        if (clients[i].fd == sock)
        {
            return i;
        }
    }

    return -1;
}

SSL* create_ssl(struct ssl_client* p, int client_sock)
{
    memset(p, 0, sizeof(struct ssl_client));

    p->fd = (int)client_sock;
    p->rbio = BIO_new(BIO_s_mem());
    p->wbio = BIO_new(BIO_s_mem());
    p->ssl = SSL_new(ctx);

    // server: SSL_set_accept_state || client: SSL_set_connect_state
    SSL_set_accept_state(p->ssl);
    SSL_set_bio(p->ssl, p->rbio, p->wbio);
    SSL_set_fd(p->ssl, client_sock);
    
    return p->ssl;
}
