#include "server.h"
#include "server_socket.h"
#include "server_tls.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>

int main(void)
{
    int serv_sock = 0;
    int client_sock = 0;
    int top = 0;
    char sock_read_buf[BUF_SIZE] = { 0, };
    fd_set read_fd = { 0, }, tmp_read_fd = { 0, };
    int fd_max = 0, fd_num = 0;
    int curr_sock = 0;
    struct timeval timeout = { 0, };
    SSL* ssl = NULL;
    int top_client = 0;
    ssl_client clients[SOCK_SIZE] = { 0, };

    // bind and listen
    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    bind_serv_sock(serv_sock);
    listen(serv_sock, 10);
    printf("Successfully bind and listening....\n\n");

    // initialize file descriptors of sockets
    fd_max = serv_sock;
    FD_ZERO(&read_fd);
    FD_SET(serv_sock, &read_fd);

    ssl_init();
    
    while (1) 
    {
        // I/O multiplexing; select function
        tmp_read_fd = read_fd;
        timeout.tv_sec = 5;
        fd_num = select(fd_max + 1, &tmp_read_fd, NULL, NULL, &timeout);
        if (fd_num <= 0) 
        {
            continue;
        }
        
        for (int i = 1; i <= fd_max; i++) 
        {
            curr_sock = i;
            if (FD_ISSET(curr_sock, &tmp_read_fd)) 
            {
                // connect with client
                if (curr_sock == serv_sock) 
                {
                    client_sock = accept_and_create_client_sock(serv_sock);
                    ssl = create_ssl(&clients[top], client_sock);
                    top++;
                    FD_SET(client_sock, &read_fd);

                    // update fd_max
                    if (fd_max < client_sock)
                    {
                        fd_max = client_sock;
                    }
                } 
                // communication with client
                else if (echo(clients, &read_fd, curr_sock, sock_read_buf, fd_max, serv_sock, top) != 0)
                {
                    // connection is closed(active close) or error occurs 
                    if (remove_client(clients, curr_sock, top)) {
                        perror("Cannot find client to be removed.\n");
                    }
                    top--;
                    close(curr_sock);
                    FD_CLR(curr_sock, &read_fd);
                    printf("Closed with connection of socket << %d >>\n\n", curr_sock);
                    continue;
                }
            }
        }
    }

    close(serv_sock);
    
    return 0;
}

void attach_noti(char* write_buf, char* buf, int sock)
{
    memset(write_buf, 0, BUF_SIZE);
    snprintf(write_buf, BUF_SIZE, "[This is from socket %d] > ", (int)sock);
    strcat(write_buf, buf);
}

int remove_client(ssl_client* clients, int cli_fd, int top) 
{
    for (int i = 0; i <= top; i++)
    {
        if (clients[i].fd == cli_fd)
        {
            for (int j = i+1; j <= top; j++) {
                clients[j-1] = clients[j];
            }
            memset(&clients[top], 0, sizeof(ssl_client));
            return 1;
        }
    }

    return 0;
}

int echo(
        ssl_client* clients,
        fd_set* read_fd,
        int curr_sock,
        char* buf, 
        int fd_max, 
        int serv_sock,
        int top) 
{
    int index = find_index_sock(clients, curr_sock, top);

    memset(buf, 0, BUF_SIZE);
    ssize_t bytes_received = SSL_read(clients[index].ssl, buf, BUF_SIZE);
    if (bytes_received <= 0) 
    {
        return -1;
    }

    // printf("[From socket %d] recv return value: %zd", client_sock, read_return);
    if (strcmp(buf, "!q") == 0) 
    {
        return -1;
    }

    if (strcmp(buf, "Succcessfully connected with client") == 0) 
    {
        printf("Succcessfully connected with client << socket %d >>\n", curr_sock);
        return 0;
    }

    char write_buf[BUF_SIZE];
    ssize_t bytes_sended;
    attach_noti(write_buf, buf, curr_sock);
    for (int fd = 1; fd <= fd_max; fd++) 
    {
        if (FD_ISSET(fd, read_fd) && fd != curr_sock && fd != serv_sock)
        {
            index = find_index_sock(clients, fd, top);
            bytes_sended = SSL_write(clients[index].ssl, write_buf, BUF_SIZE);
            if (bytes_sended <= 0) 
            {
                printf("Connection with client(socket %d) is closed.\n", fd);
                continue;
            }
        }
    }
    
    return 0;
}
