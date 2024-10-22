#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

#include "tcpsocket_tls.h"

#define BUF_SIZE 256
#define SOCK_SIZE 10

SSL_CTX* ctx;
int serv_sock;
int fd_max;
int tls_sockets_top;
tls_socket tls_sockets[SOCK_SIZE];

void error_handle(char* msg) 
{
    printf("%s\n\n", msg);
    exit(-1);
}

void show_last_error_msg() 
{
    char msg_buf[BUF_SIZE] = { 0, };
    unsigned long err_num = ERR_get_error();

    printf("\n------ ERROR ------\n");
    printf("err_num : %ld\n", err_num);
    ERR_error_string(err_num, msg_buf);
    printf("err: %s\n", msg_buf);
}

void attach_prefix(char* sock_write_buf, char* sock_read_buf, int sock) 
{
    memset(sock_write_buf, 0, BUF_SIZE);
    snprintf(sock_write_buf, BUF_SIZE, "[This is from socket %d] > ", (int)sock);
    strcat(sock_write_buf, sock_read_buf);
}

int remove_client(int cli_fd, int top) 
{
    for (int i = 0; i <= top; i++) {
        if (tls_sockets[i].fd == cli_fd) {
            for (int j = i+1; j <= top; j++) {
                tls_sockets[j-1] = tls_sockets[j];
            }
            memset(&tls_sockets[top], 0, sizeof(tls_socket));
            return 0;
        }
    }

    return 1;
}

int find_index_sock(int sock, int top) {
    for (int i = 0; i <= top; i++) {
        if (tls_sockets[i].fd == sock) {
            return i;
        }
    }

    return -1;
}

int echo(fd_set* read_fd, int curr_sock, char* sock_read_buf) 
{
    char sock_write_buf[BUF_SIZE];
    ssize_t bytes_sended;
    
    int index = find_index_sock(curr_sock, tls_sockets_top);

    /* read socket and decrypt */
    memset(sock_read_buf, 0, BUF_SIZE);
    ssize_t bytes_received = SSL_read(tls_sockets[index].ssl, sock_read_buf, BUF_SIZE);
    if (bytes_received <= 0) {
        return -1;
    }
    // printf("[From socket %d] recv return value: %zd", client_sock, read_return);

    if (strcmp(sock_read_buf, "!q") == 0) {
        return -1;
    }

    /* check if a message is for init of connection. */
    if (strcmp(sock_read_buf, "Succcessfully connected with client") == 0) {
        printf("Succcessfully connected with client << socket %d >>\n", curr_sock);
        return 0;
    }

    /* print message received */
    printf("Receive: message from socket \"%d\": %s\n", curr_sock, sock_read_buf);

    /* distribute message to sockets that are not sender. */
    attach_prefix(sock_write_buf, sock_read_buf, curr_sock);
    for (int fd = 3; fd <= fd_max; fd++) {
        if (FD_ISSET(fd, read_fd) && fd != curr_sock && fd != serv_sock) {
            index = find_index_sock(fd, tls_sockets_top);
            if (index < 0) {
                error_handle("Cannot find client socket.");
            }

            bytes_sended = SSL_write(tls_sockets[index].ssl, sock_write_buf, BUF_SIZE);
            
            /* if connection of client closed, this socket will be retrieved in loop of caller */
            if (bytes_sended <= 0) {
                printf("Connection with client(socket %d) is closed.\n", fd);
                continue;
            }
            printf("Send: message to socket \"%d\": %s\n", fd, sock_read_buf);
        }
    }
    return 0;
    
}

int main(void) 
{
    char sock_read_buf[BUF_SIZE] = { 0, };
    fd_set read_fd = { 0, };
    fd_set tmp_read_fd = { 0, };
    struct timeval timeout = { 0, };

    /* information of binding port and certificate path. */
    int port = 443;
    char certificate_path[BUF_SIZE] = { "../certificate/server.crt" };
    char key_path[BUF_SIZE] = { "../certificate/server.key" };

    ssl_init();
    ctx = create_server_ssl_ctx(certificate_path, key_path);

    /* bind and listen */
    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    bind_serv_sock(serv_sock, port);
    listen(serv_sock, 10);
    printf("Successfully bind and listening....\n\n");

    /* initialize a set of file descriptors of sockets, it would be waiting list in select function */
    fd_max = serv_sock;
    FD_ZERO(&read_fd);
    FD_SET(serv_sock, &read_fd);

    while (1) {
        /* I/O multiplexing; select function */
        tmp_read_fd = read_fd;
        timeout.tv_sec = 5;
        int fd_num = select(fd_max + 1, &tmp_read_fd, NULL, NULL, &timeout);
        if (fd_num <= 0) {
            continue;
        }
        
        for (int i = 3; i <= fd_max; i++) {
            int curr_sock = i;
            if (FD_ISSET(curr_sock, &tmp_read_fd)) {
                /* connect with client */
                if (curr_sock == serv_sock) {
                    int client_sock = accept_and_create_client_sock(serv_sock);
                    create_server_ssl(ctx, &tls_sockets[tls_sockets_top], client_sock);
                    tls_sockets_top++;
                    FD_SET(client_sock, &read_fd);

                    /* update fd_max */
                    if (fd_max < client_sock) {
                        fd_max = client_sock;
                    }
                } 
                /* communication with client */
                else if (echo(&read_fd, curr_sock, sock_read_buf) != 0) {
                    /* connection is closed(active close) or error occurs */
                    if (remove_client(curr_sock, tls_sockets_top)) {
                        perror("Cannot find client to be removed.\n");
                    }
                    tls_sockets_top--;
                    close(curr_sock);
                    FD_CLR(curr_sock, &read_fd);
                    printf("Closed with connection of socket << %d >>\n", curr_sock);
                    continue;
                }
            }
        }
    }

    close(serv_sock);
    
    return 0;
}
