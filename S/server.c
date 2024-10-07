#include "server.h"

int main(void)
{
	int serv_sock = 0;
	int client_sock = 0;
	int top = 0;
	int client_socks[SOCK_SIZE] = { 0, };
	char sock_read_buf[BUF_SIZE] = { 0, };

	serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	bind_serv_sock(serv_sock);
	listen(serv_sock, 10);

	while (1) {
		client_sock = accept_and_create_client_sock(serv_sock);
		client_socks[top] = client_sock;
		top++;
	
		if (echo(client_socks, client_sock, sock_read_buf) != 0)
			break;
	}

	close(serv_sock);
	close(client_sock);
	
	return 0;
}