#include "client.h"
#include "client_socket.h"
#include "client_tls.h"
#include "buffer_config.h"
#include "error_handler.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int main(void)
{
    int client_sock = 0;
    char stdin_read_buf[BUF_SIZE] = { 0, };
    char sock_read_buf[BUF_SIZE] = { 0, };
    SSL_CTX* ctx = NULL;
    SSL* ssl = NULL;
    ssl_client client = { 0, };

    client_sock = create_sock();
    connect_with_serv(client_sock);

    // create SSL and config
    ssl_init();
    ctx = create_ssl_ctx();
    ssl_ctx_config(ctx);
    ssl = create_ssl(&client, ctx, client_sock);

    do_ssl_handshake(&client);

    // create thread for reading socket buffer
    pthread_t thread_1;
    read_sock_t rs = { &client, sock_read_buf };
    if (pthread_create(&thread_1, NULL, read_socket, (void*)&rs) != 0) 
    {
        close(client_sock);
        error_handle("Failed to create thread.");
    }

    // echo client
    while (1)
    {
        if (echo(&client, client_sock, stdin_read_buf, sock_read_buf) != 0)
        {
            break;
        }
    }

    close(client_sock);

    return 0;
}

int echo(ssl_client* client, int client_sock, char* send_msg_buf, char* recv_msg_buf) 
{
    memset(send_msg_buf, 0, BUF_SIZE);
    if (fgets(send_msg_buf, BUF_SIZE, stdin) == NULL)
    {
        error_handle("Error occurs when reading stdin buffer.\n\n");
    }

    // remove new line character
    if (send_msg_buf[0] == 0) // check if char array is empty
    {
        error_handle("Message buffer for writing is empty\n\n");
    }
    size_t ln = strlen(send_msg_buf) - 1;
    if (*send_msg_buf && send_msg_buf[ln] == '\n') 
    {
        send_msg_buf[ln] = '\0';
    }

    SSL_write(client->ssl, send_msg_buf, BUF_SIZE);
    if (strcmp(send_msg_buf, "!q") == 0)
    {
        return -1;
    }

    return 0;
}
