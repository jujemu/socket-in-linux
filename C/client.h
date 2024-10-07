#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <pthread.h>

#define BUF_SIZE 256
#define PORT 443
#define SERV_IP_ADDR "127.0.0.1"

void error_handle(char* msg) {
    printf("%s\n\n", msg);
    exit(-1);
}

void connect_with_serv(int client_sock) {
    struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	inet_pton(AF_INET, SERV_IP_ADDR, &serv_addr.sin_addr.s_addr);
	serv_addr.sin_port = htons(PORT);
	connect(client_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
}

int echo(int client_sock, char* send_msg_buf, char* recv_msg_buf) {
    memset(send_msg_buf, 0, BUF_SIZE);
    if (fgets(send_msg_buf, BUF_SIZE, stdin) == NULL)
        error_handle("Error occurs when reading stdin buffer.\n\n");

    if (send_msg_buf[0] == 0)
        error_handle("Message buffer for writing is empty\n\n");

    // remove new line character
    size_t ln = strlen(send_msg_buf) - 1;
    if (*send_msg_buf && send_msg_buf[ln] == '\n') 
        send_msg_buf[ln] = '\0';

    write(client_sock, send_msg_buf, BUF_SIZE);
    if (strcmp(send_msg_buf, "!q") == 0)
        return -1;
    
    return 0;
}