#define PORT 443

void bind_serv_sock(int serv_sock);
int accept_and_create_client_sock(int serv_sock);
