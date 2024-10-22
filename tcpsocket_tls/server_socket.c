#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

void bind_serv_sock(int serv_sock, int port) {
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);
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
    
    return client_sock;
}
