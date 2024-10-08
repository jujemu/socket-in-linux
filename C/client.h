#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>

#include <pthread.h>

#define BUF_SIZE 256
#define PORT 443
#define SERV_IP_ADDR "127.0.0.1"
#define CA_CERT_PATH "/root/projects/echo/C/rootca.crt"

enum sslstatus 
{ 
    SSLSTATUS_OK, SSLSTATUS_WANT_IO, SSLSTATUS_FAIL 
};

struct ssl_client
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
} client;

typedef struct read_sock_t {
	struct ssl_client* client;
	char* buf;
} read_sock_t;

void error_handle(char* msg) {
    printf("%s\n\n", msg);
    exit(-1);
}

void ssl_init() {
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

SSL* create_ssl(struct ssl_client* p,
    SSL_CTX* ctx,
    int sock)
{
    memset(p, 0, sizeof(struct ssl_client));
    p->fd = sock;
    p->rbio = BIO_new(BIO_s_mem());
    p->wbio = BIO_new(BIO_s_mem());
    p->ssl = SSL_new(ctx);

    // server: SSL_set_accept_state || client: SSL_set_connect_state
    SSL_set_connect_state(p->ssl);
    SSL_set_bio(p->ssl, p->rbio, p->wbio);
    // wrap the socket with SSL
    SSL_set_fd(client.ssl, sock);
    return p->ssl;
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

void print_ssl_state()
{
    const char* current_state = SSL_state_string_long(client.ssl);
    if (current_state != client.last_state) {
        if (current_state)
            printf("SSL-STATE: %s\n", current_state);
        client.last_state = current_state;
    }
}

void queue_encrypted_bytes(const char* buf, size_t len)
{
    client.write_buf = (char*)realloc(client.write_buf, client.write_len + len);
    memcpy(client.write_buf + client.write_len, buf, len);
    client.write_len += len;
}

enum sslstatus do_ssl_handshake()
{
    char buf[BUF_SIZE];
    char msg[BUF_SIZE] = { "Succcessfully connected with client" };
    enum sslstatus status;

    print_ssl_state();
    int n = SSL_do_handshake(client.ssl);
    print_ssl_state();
    printf("\n");
    status = get_sslstatus(client.ssl, n);

    /* Did SSL request to write bytes? */
    if (status == SSLSTATUS_WANT_IO)
        do {
            n = BIO_read(client.wbio, buf, sizeof(buf));
            if (n > 0)
                queue_encrypted_bytes(buf, n);
            else if (!BIO_should_retry(client.wbio))
                return SSLSTATUS_FAIL;
        } while (n > 0);

    /* TLS handshake 이후에 init 문자열 전송 */
    SSL_write(client.ssl, msg, strlen(msg)+1);

    return status;
}

void connect_with_serv(int client_sock) {
    struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	inet_pton(AF_INET, SERV_IP_ADDR, &serv_addr.sin_addr.s_addr);
	serv_addr.sin_port = htons(PORT);
	if (connect(client_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0) 
        error_handle("Cannot connect with server.");
}

int echo(struct ssl_client* client, int client_sock, char* send_msg_buf, char* recv_msg_buf) {
    memset(send_msg_buf, 0, BUF_SIZE);
    if (fgets(send_msg_buf, BUF_SIZE, stdin) == NULL)
        error_handle("Error occurs when reading stdin buffer.\n\n");

    // remove new line character
    if (send_msg_buf[0] == 0) // check if char array is empty
        error_handle("Message buffer for writing is empty\n\n");
    size_t ln = strlen(send_msg_buf) - 1;
    if (*send_msg_buf && send_msg_buf[ln] == '\n') 
        send_msg_buf[ln] = '\0';

    SSL_write(client->ssl, send_msg_buf, BUF_SIZE);
    if (strcmp(send_msg_buf, "!q") == 0)
        return -1;
    
    return 0;
}

void *read_socket(void *param) {
	read_sock_t* rs = (read_sock_t*) param;
	while (1) {
		ssize_t bytes_received = SSL_read(rs->client->ssl, rs->buf, BUF_SIZE);
		
		if (bytes_received == 0)
			error_handle("Server connection closed.");
			
		if (bytes_received < 0)
			error_handle("[Error] Reading socket.");

    	printf("%s\n", rs->buf);
	}
	return NULL;
}