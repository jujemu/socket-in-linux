#ifndef SERVER_TLS_H
#define SERVER_TLS_H
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

void server_ssl_init(char* certificate_path, char* key_path);
int find_index_sock(ssl_client* clients, int sock, int top);
SSL* server_create_ssl(struct ssl_client* p, int client_sock);
