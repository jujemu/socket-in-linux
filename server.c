#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>


#define BUF_SIZE 256
#define PORT 443

int main(void)
{
	int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);
	if (bind(listen_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0) 
	{
		printf("Bind fails.\n");
		close(listen_sock);
		exit(1);
	}

	listen(listen_sock, 10);

	struct sockaddr_in client_addr;
	int size_client_addr = sizeof(client_addr);
	int client_sock = accept(listen_sock, (struct sockaddr*)&client_addr, &size_client_addr);
    printf("Connected with client socket << %d >>\n\n", client_sock);
	
	char buf[BUF_SIZE];
	while (1)
	{
		if (recv(client_sock, buf, BUF_SIZE, 0) < 0) {
            printf("Closed client socket.\n");
            break;
        }

		if (strcmp(buf, "!q") == 0)
			break;
		send(client_sock, buf, BUF_SIZE, 0);
	}

	close(listen_sock);
	close(client_sock);
	return 0;
}