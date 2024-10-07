#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define BUF_SIZE 256
#define PORT 443

void bind_serv_sock(int serv_sock){
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

int echo(int client_sock, char* buf) {
    ssize_t read_return = read(client_sock, buf, BUF_SIZE);
    if (read_return <= 0) {
            printf("Closed client socket.\n");
            return -1;
        }

    printf("recv return value: %zd", read_return);
    if (strcmp(buf, "!q") == 0)
        return -1;

    write(client_sock, buf, BUF_SIZE);
    return 0;
}