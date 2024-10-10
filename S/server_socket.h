#define BUF_SIZE 256
#define PORT 443
#define SOCK_SIZE 10

void bind_serv_sock(int serv_sock);
int accept_and_create_client_sock(int serv_sock);
