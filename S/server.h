#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>

#define BUF_SIZE 256
#define PORT 443
#define SOCK_SIZE 10
#define CERTIFICATE_PATH "/root/projects/echo/S/server.crt"
#define KEY_PATH "/root/projects/echo/S/server.key"

void bind_serv_sock(int serv_sock) {
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);
    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0) {
        printf("Bind fails.\n");
        close(serv_sock);
        exit(1);
    }
}

int accept_and_create_client_sock(int serv_sock) {
    struct sockaddr_in client_addr;
	int size_client_addr = sizeof(client_addr);
	int client_sock = accept(serv_sock, (struct sockaddr*)&client_addr, &size_client_addr);
    printf("Connected with client socket << %d >>\n\n", client_sock);
    
    return client_sock;
}

void attach_noti(char* write_buf, char* buf, int sock)
{
	memset(write_buf, 0, BUF_SIZE);
	snprintf(write_buf, BUF_SIZE, "[This is from socket %d] > ", (int)sock);
	strcat(write_buf, buf);
}

SSL_CTX* ctx;

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
		perror("SSL context 생성에 문제가 생겼습니다.");
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

int find_index_sock(ssl_client* clients, int sock, int top) {
	for (int i = 0; i <= top; i++) {
		if (clients[i].fd == sock)
			return i;
	}
	return -1;
}

SSL* create_ssl(struct ssl_client* p,
	int client_sock)
{
	memset(p, 0, sizeof(struct ssl_client));

	p->fd = (int)client_sock;
	p->rbio = BIO_new(BIO_s_mem());
	p->wbio = BIO_new(BIO_s_mem());
	p->ssl = SSL_new(ctx);

	// server: SSL_set_accept_state || client: SSL_set_connect_state
    SSL_set_accept_state(p->ssl);
	SSL_set_bio(p->ssl, p->rbio, p->wbio);
	//SSL_accept(p->ssl);
	SSL_set_fd(p->ssl, client_sock);
	return p->ssl;
}

int echo(
        ssl_client* clients,
        fd_set* read_fd,
        int curr_sock,
        char* buf, 
        int fd_max, 
        int serv_sock,
        int top) 
{
    int index = find_index_sock(clients, curr_sock, top);

    memset(buf, 0, BUF_SIZE);
    ssize_t bytes_received = SSL_read(clients[index].ssl, buf, BUF_SIZE);
    if (bytes_received <= 0) {
        printf("Closed client socket.\n");
        return -1;
    }

    // printf("[From socket %d] recv return value: %zd", client_sock, read_return);
    if (strcmp(buf, "!q") == 0)
        return -1;

    if (strcmp(buf, "Succcessfully connected with client") == 0) {
        printf("Succcessfully connected with client << socket %d >>\n", curr_sock);
        return 0;
    }

    char write_buf[BUF_SIZE];
    ssize_t bytes_sended;
    attach_noti(write_buf, buf, curr_sock);
    for (int fd = 1; fd <= fd_max; fd++) {
        if (FD_ISSET(fd, read_fd) && fd != curr_sock && fd != serv_sock){
            index = find_index_sock(clients, fd, top);
            bytes_sended = SSL_write(clients[index].ssl, write_buf, BUF_SIZE);
            if (bytes_sended <= 0) {
                printf("Connection with client(socket %d) is closed.\n", fd);
                continue;
            }
        }
    }
    
    return 0;
}