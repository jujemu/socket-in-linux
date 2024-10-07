#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 256
#define PORT 443
#define SERV_IP_ADDR "127.0.0.1"

int main(void)
{
	int client_sock = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	inet_pton(AF_INET, SERV_IP_ADDR, &serv_addr.sin_addr.s_addr);
	serv_addr.sin_port = htons(PORT);
	connect(client_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	char buf[BUF_SIZE];
	char echo[BUF_SIZE];
	while (1)
	{
		printf("%s", "> ");
		fgets(buf, BUF_SIZE, stdin);
		send(client_sock, buf, BUF_SIZE, 0);
		if (strcmp(buf, "!q") == 0)
			break;

		recv(client_sock, echo, BUF_SIZE, 0);
		printf("%s\n", echo);
	}

	close(client_sock);

	return 0;
}