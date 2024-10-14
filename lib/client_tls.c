#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include "client_tls.h"
#include "config.h"

void ssl_init()
{
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ERR_load_crypto_strings();
}

/* 라이브러리가 core key를 관리하고 사용자가 접근할 수 있도록 context를 제공한다. */
SSL_CTX* create_ssl_ctx()
{
    const SSL_METHOD* method = TLS_method();
    SSL_CTX* ctx = SSL_CTX_new(method);
    if (!ctx)
    {
        perror("SSL context 생성에 문제가 생겼습니다.");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

/* server.cert를 만들 때, 같이 넣어주었던 CA cert로 검증할 수 있도록 한다. */
void ssl_ctx_config(SSL_CTX* ctx)
{
    //Recommended to avoid SSLv2 & SSLv3
    SSL_CTX_set_options(ctx, SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);
    SSL_CTX_set_verify_depth(ctx, 1);
    SSL_CTX_load_verify_locations(ctx, CA_CERT_PATH, NULL);
}

SSL* create_ssl(ssl_client* p,
    SSL_CTX* ctx,
    int sock)
{
    memset(p, 0, sizeof(ssl_client));
    p->fd = sock;
    p->rbio = BIO_new(BIO_s_mem());
    p->wbio = BIO_new(BIO_s_mem());
    p->ssl = SSL_new(ctx);

    // server: SSL_set_accept_state || client: SSL_set_connect_state
    SSL_set_connect_state(p->ssl);
    SSL_set_bio(p->ssl, p->rbio, p->wbio);
    // wrap the socket with SSL
    SSL_set_fd(p->ssl, sock);
    return p->ssl;
}

void print_ssl_state(ssl_client* client)
{
    const char* current_state = SSL_state_string_long(client->ssl);
    if (current_state != client->last_state) 
    {
        if (current_state)
            printf("SSL-STATE: %s\n", current_state);
        client->last_state = current_state;
    }
}

static enum sslstatus get_sslstatus(SSL* ssl, int n)
{
    switch (SSL_get_error(ssl, n))
    {
    case SSL_ERROR_NONE:
        return SSLSTATUS_OK;
    case SSL_ERROR_WANT_WRITE:
    case SSL_ERROR_WANT_READ:
        return SSLSTATUS_WANT_IO;
    case SSL_ERROR_ZERO_RETURN:
    case SSL_ERROR_SYSCALL:
    default:
        return SSLSTATUS_FAIL;
    }
}

void queue_encrypted_bytes(ssl_client* client, const char* buf, size_t len)
{
    client->write_buf = (char*)realloc(client->write_buf, client->write_len + len);
    memcpy(client->write_buf + client->write_len, buf, len);
    client->write_len += len;
}

enum sslstatus do_ssl_handshake(ssl_client* client)
{
    char buf[BUF_SIZE];
    char msg[BUF_SIZE] = { "Succcessfully connected with client" };
    enum sslstatus status;

    print_ssl_state(client);
    int n = SSL_do_handshake(client->ssl);
    print_ssl_state(client);
    printf("\n");
    status = get_sslstatus(client->ssl, n);

    /* Did SSL request to write bytes? */
    if (status == SSLSTATUS_WANT_IO) 
    {
        do 
        {
            n = BIO_read(client->wbio, buf, sizeof(buf));
            if (n > 0) 
            {
                queue_encrypted_bytes(client, buf, n);
            }
            else if (!BIO_should_retry(client->wbio))
            {
                return SSLSTATUS_FAIL;
            }
        } while (n > 0);
    }
    
    /* TLS handshake 이후에 init 문자열 전송 */
    SSL_write(client->ssl, msg, strlen(msg)+1);

    return status;
}
