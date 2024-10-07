#include "client.h"

typedef struct read_sock_t {
	int client_sock;
	char* buf;
} read_sock_t;

void *read_socket(void *param) {
	read_sock_t* rs = (read_sock_t*) param;
	while (1) {
		ssize_t bytes_received = read(rs->client_sock, rs->buf, BUF_SIZE);
		
		if (bytes_received == 0)
			error_handle("Server connection closed.");
			
		if (bytes_received < 0)
			error_handle("[Error] Reading socket.");

    	printf("%s\n", rs->buf);
	}
	return NULL;
}

int main(void)
{
	int client_sock = socket(AF_INET, SOCK_STREAM, 0);

	connect_with_serv(client_sock);

	char stdin_read_buf[BUF_SIZE] = { 0, };
	char sock_read_buf[BUF_SIZE] = { 0, };

	pthread_t thread_1;
	read_sock_t rs = { client_sock, sock_read_buf };
	if (pthread_create(&thread_1, NULL, read_socket, (void*)&rs) != 0) {
		close(client_sock);
		error_handle("Failed to create thread.");
	}

	while (1)
		if (echo(client_sock, stdin_read_buf, sock_read_buf) != 0)
			break;

	close(client_sock);

	return 0;
}