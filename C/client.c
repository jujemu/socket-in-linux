#include "client.h"

int main(void)
{
	int client_sock = socket(AF_INET, SOCK_STREAM, 0);

	connect_with_serv(client_sock);

	char stdin_read_buf[BUF_SIZE];
	char sock_read_buf[BUF_SIZE];
	while (1)
		if (echo(client_sock, stdin_read_buf, sock_read_buf) != 0)
			break;	

	close(client_sock);

	return 0;
}