#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 256
#define PORT 443
#define SERV_IP_ADDR "127.0.0.1"

void connect_with_serv(int client_sock) {
    struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	inet_pton(AF_INET, SERV_IP_ADDR, &serv_addr.sin_addr.s_addr);
	serv_addr.sin_port = htons(PORT);
	connect(client_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
}

int echo(int client_sock, char* send_msg_buf, char* recv_msg_buf) {
    printf("%s", "> ");
    fgets(send_msg_buf, BUF_SIZE, stdin);
    send(client_sock, send_msg_buf, BUF_SIZE, 0);
    if (strcmp(send_msg_buf, "!q") == 0)
        return -1;

    recv(client_sock, recv_msg_buf, BUF_SIZE, 0);
    printf("%s\n", recv_msg_buf);

    return 0;
}