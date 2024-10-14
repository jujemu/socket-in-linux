#include <openssl/ssl.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "config.h"
#include "error_handler.h"
#include "client_socket.h"
#include "client_tls.h"

int create_sock()
{
    return socket(AF_INET, SOCK_STREAM, 0);
}

void connect_with_serv(int client_sock)
{
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, SERV_IP_ADDR, &serv_addr.sin_addr.s_addr);
    serv_addr.sin_port = htons(PORT);
    if (connect(client_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0) 
    {
        error_handle("Cannot connect with server.");
    }
}

void *read_socket(void *param)
{
    read_sock_t* rs = (read_sock_t*) param;
    while (1) 
    {
        ssize_t bytes_received = SSL_read(rs->client->ssl, rs->buf, BUF_SIZE);
        
        if (bytes_received == 0)
        {
            error_handle("Server connection closed.");
        }
            
        if (bytes_received < 0)
        {
            error_handle("[Error] Reading socket.");
        }

        printf("%s\n", rs->buf);
    }
    
    return NULL;
}
