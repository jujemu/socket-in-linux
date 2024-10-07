#include "server.h"

int main(void)
{
	int serv_sock = 0;
	int client_sock = 0;
	int top = 0;
	// int client_socks[SOCK_SIZE] = { 0, };
	char sock_read_buf[BUF_SIZE] = { 0, };
	fd_set read_fd = { 0, }, tmp_read_fd = { 0, };
	int fd_max = 0, fd_num = 0;
	int curr_sock = 0;
	struct timeval timeout = { 0, };

	serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	bind_serv_sock(serv_sock);
	listen(serv_sock, 10);
	printf("Successfully bind and listening....\n");

	// while (1) {
	// 	client_sock = accept_and_create_client_sock(serv_sock);
	// 	client_socks[top] = client_sock;
	// 	top++;
	
	// 	if (echo(client_socks, client_sock, sock_read_buf) != 0)
	// 		break;
	// }

	fd_max = serv_sock;
	FD_ZERO(&read_fd);
	FD_SET(serv_sock, &read_fd);
	while (1) {
		tmp_read_fd = read_fd;
		timeout.tv_sec = 5;
		fd_num = select(fd_max + 1, &tmp_read_fd, NULL, NULL, &timeout);
		if (fd_num <= 0)
			continue;
		
		for (int i = 1; i <= fd_max; i++) {
			curr_sock = i;
			if (FD_ISSET(curr_sock, &tmp_read_fd)) {
				if (curr_sock == serv_sock) {
					client_sock = accept_and_create_client_sock(serv_sock);

					if (fd_max < client_sock)
						fd_max = client_sock;

					FD_SET(client_sock, &read_fd);
					// client_socks[top] = client_sock;
					// top++;
				} else {
					if (echo(&read_fd, curr_sock, sock_read_buf, fd_max, serv_sock) != 0){
						close(curr_sock);
						FD_CLR(curr_sock, &read_fd);
						printf("Closed with connection of socket %d\n", curr_sock);
						// for (int j = 0; j <= top; j++) {
						// 	if (client_socks[j] == curr_sock)
						// 		for (int k = j; k < top; k++) 
						// 			client_socks[k] = client_socks[k+1];
						// }
						// top--;
						continue;
					}
				}
			}
		}
	}

	close(serv_sock);
	
	return 0;
}