#include "server.h"

int main(void)
{
	int serv_sock = 0;
	int client_sock = 0;
	int top = 0;
	char sock_read_buf[BUF_SIZE] = { 0, };
	fd_set read_fd = { 0, }, tmp_read_fd = { 0, };
	int fd_max = 0, fd_num = 0;
	int curr_sock = 0;
	struct timeval timeout = { 0, };

	// bind and listen
	serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	bind_serv_sock(serv_sock);
	listen(serv_sock, 10);
	printf("Successfully bind and listening....\n");

	// initialize file descriptors
	fd_max = serv_sock;
	FD_ZERO(&read_fd);
	FD_SET(serv_sock, &read_fd);
	
	while (1) {
		// I/O multiplexing; select func
		tmp_read_fd = read_fd;
		timeout.tv_sec = 5;
		fd_num = select(fd_max + 1, &tmp_read_fd, NULL, NULL, &timeout);
		if (fd_num <= 0)
			continue;
		
		for (int i = 1; i <= fd_max; i++) {
			curr_sock = i;
			if (FD_ISSET(curr_sock, &tmp_read_fd)) {
				// connect with client
				if (curr_sock == serv_sock) {
					client_sock = accept_and_create_client_sock(serv_sock);

					// update fd_max
					if (fd_max < client_sock)
						fd_max = client_sock;

					FD_SET(client_sock, &read_fd);
				// communication with client
				} else {
					if (echo(&read_fd, curr_sock, sock_read_buf, fd_max, serv_sock) != 0){
						// connection is closed(active close) or error occurs
						close(curr_sock);
						FD_CLR(curr_sock, &read_fd);
						printf("Closed with connection of socket %d\n\n", curr_sock);
						continue;
					}
				}
			}
		}
	}

	close(serv_sock);
	
	return 0;
}