#include "client_socket.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <openssl/ssl.h>

#include "error_handler.h"

int create_tcp_sock() {
    return socket(AF_INET, SOCK_STREAM, 0);
}

void connect_with_serv(int client_sock, int port, char *serv_ip_addr) {
    if (port <= 0 && 65535 <= port) {
        error_handle("Invalid port.");
    }
    if (strcmp(serv_ip_addr, "localhost") == 0) {
        serv_ip_addr = "127.0.0.1";
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, serv_ip_addr, &serv_addr.sin_addr.s_addr);
    serv_addr.sin_port = htons(port);
    if (connect(client_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0) {
        error_handle("Cannot connect with server.");
    }
}
