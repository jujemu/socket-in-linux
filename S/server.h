#include "server_tls.h"

#define BUF_SIZE 256
#define PORT 443
#define SOCK_SIZE 10
#define CERTIFICATE_PATH "/root/projects/echo/S/certificate/server.crt"
#define KEY_PATH "/root/projects/echo/S/certificate/server.key"

int echo(ssl_client* clients, fd_set* read_fd, int curr_sock, char* buf, int fd_max, int serv_sock, int top);
void attach_noti(char* write_buf, char* buf, int sock);
