#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define BUF_SIZE 256
#define PORT 443
#define SOCK_SIZE 10

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

int echo(fd_set* read_fd, int curr_sock, char* buf, int fd_max, int serv_sock) {
    memset(buf, 0, BUF_SIZE);
    ssize_t read_return = read(curr_sock, buf, BUF_SIZE);
    if (read_return <= 0) {
        printf("Closed client socket.\n");
        return -1;
    }

    // printf("[From socket %d] recv return value: %zd", client_sock, read_return);
    if (strcmp(buf, "!q") == 0)
        return -1;

    for (int fd = 1; fd <= fd_max; fd++) {
        if (FD_ISSET(fd, read_fd) && fd != curr_sock && fd != serv_sock)
            write(fd, buf, BUF_SIZE);
    }
    
    return 0;
}