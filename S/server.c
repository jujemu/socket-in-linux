#include "server.h"

int main(void)
{
	int serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	bind_serv_sock(serv_sock);
	listen(serv_sock, 10);

	int client_sock = accept_and_create_client_sock(serv_sock);
	
	char sock_read_buf[BUF_SIZE];
	while (1) 
		if (echo(client_sock, sock_read_buf) != 0)
			break;

	close(serv_sock);
	close(client_sock);
	
	return 0;
}