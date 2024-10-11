#include "server_tls.h"

#define BUF_SIZE 256
#define SOCK_SIZE 10

int echo(ssl_client* clients, fd_set* read_fd, int curr_sock, char* buf, int fd_max, int serv_sock, int top);
int remove_client(ssl_client* clients, int cli_fd, int top);
void attach_noti(char* write_buf, char* buf, int sock);
