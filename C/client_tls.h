#include <openssl/ssl.h>

#define CA_CERT_PATH "/root/projects/echo/C/certificate/rootca.crt"

#ifndef CLIENT_TLS_H
#define CLIENT_TLS_H
enum sslstatus 
{ 
    SSLSTATUS_OK, SSLSTATUS_WANT_IO, SSLSTATUS_FAIL 
};

typedef struct ssl_client
{
    int fd;

    SSL* ssl;
    BIO* rbio; /* SSL reads from, we write to. */
    BIO* wbio; /* SSL writes to, we read from. */

    /* Bytes waiting to be written to socket. This is data that has been generated
     * by the SSL object, either due to encryption of user input, or, writes
     * requires due to peer-requested SSL renegotiation. */
    char* write_buf;
    size_t write_len;

    /* Bytes waiting to be encrypted by the SSL object. */
    char* encrypt_buf;
    size_t encrypt_len;

    /* Store the previous state string */
    const char* last_state;

    /* Method to invoke when unencrypted bytes are available. */
    void (*io_on_read)(char* buf, size_t len);
} ssl_client;
#endif

static enum sslstatus get_sslstatus(SSL* ssl, int n);
void ssl_init();
SSL_CTX* create_ssl_ctx();
void ssl_ctx_config(SSL_CTX* ctx);
SSL* create_ssl(struct ssl_client*, SSL_CTX* ctx, int sock);
void print_ssl_state();
void queue_encrypted_bytes(struct ssl_client*, const char* buf, size_t len);
enum sslstatus do_ssl_handshake();
