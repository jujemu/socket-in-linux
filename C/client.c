#include "client.h"

int main(void)
{
	int client_sock = 0;
	char stdin_read_buf[BUF_SIZE] = { 0, };
	char sock_read_buf[BUF_SIZE] = { 0, };
	SSL_CTX* ctx = NULL;
	SSL* ssl = NULL;

	client_sock = socket(AF_INET, SOCK_STREAM, 0);
	connect_with_serv(client_sock);

	// create SSL and config
	ssl_init();
	ctx = create_ssl_ctx();
	ssl_ctx_config(ctx);
	ssl = create_ssl(&client, ctx, client_sock);

	do_ssl_handshake();

	// create thread for reading socket buffer
	pthread_t thread_1;
	read_sock_t rs = { &client, sock_read_buf };
	if (pthread_create(&thread_1, NULL, read_socket, (void*)&rs) != 0) {
		close(client_sock);
		error_handle("Failed to create thread.");
	}

	// echo client
	while (1)
		if (echo(&client, client_sock, stdin_read_buf, sock_read_buf) != 0)
			break;

	close(client_sock);

	return 0;
}