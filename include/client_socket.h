#define PORT 443
#define SERV_IP_ADDR "127.0.0.1"

#ifndef CLIENT_SOCKET_H
#define CLIENT_SOCKET_H
typedef struct read_sock_t 
{
    struct ssl_client* client;
    char* buf;
} read_sock_t;
#endif

int create_sock();
void connect_with_serv(int client_sock);
void *read_socket(void *param);
