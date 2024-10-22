#define __USE_GNU // needed to include pthread_tryjoin_np
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "tcpsocket_tls.h"

#define BUF_SIZE 256

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

int send_to_serv(SSL* ssl, char* send_msg_buf) {
    memset(send_msg_buf, 0, BUF_SIZE);
    if (fgets(send_msg_buf, BUF_SIZE, stdin) == NULL) {
        show_last_error_msg();
        error_handle("Error occurs when reading stdin buffer.");
    }
    
    /* remove new line character */
    if (send_msg_buf[0] == 0) { // check if char array is empty
        error_handle("Message buffer for writing is empty");
    }
    size_t length = strlen(send_msg_buf) - 1;
    if (*send_msg_buf && send_msg_buf[length] == '\n') {
        send_msg_buf[length] = '\0';
    }

    if (SSL_write(ssl, send_msg_buf, BUF_SIZE) <= 0) {
        show_last_error_msg();
        error_handle("Error occurs when sending message to server.");
    }

    if (strcmp(send_msg_buf, "!q") == 0) {
        return -1;
    }

    return 0;
}

/* if thread suddenly died, check the status of thread return. */
void check_thread_status(pthread_t* thread_1, void* thread_rtn) {
    int join_status = pthread_tryjoin_np(*thread_1, &thread_rtn); // non-blocking pthread_join

    if (join_status == 0) {
        if (thread_rtn) {
            enum thread_error_code* error_code = (enum thread_error_code*)thread_rtn;
            if (SERVER_CLOSED == *error_code) {
                error_handle("Server connection closed.");
            } else if (FAIL_TO_READ == *error_code) {
                show_last_error_msg();
                error_handle("Error occurs when reading socket.");
            } else {
                show_last_error_msg();
                error_handle("Thread died out of reading loop.");
            }
            
            thread_rtn = NULL;
        }
    } else if (join_status != EBUSY) {
        show_last_error_msg();
        error_handle("Reading thread died suddenly.");
    }
    
}

int main(void) {
    int client_sock = 0;
    SSL_CTX* ctx = NULL;
    SSL* ssl = NULL;
    char stdin_read_buf[BUF_SIZE] = { 0, };
    char sock_read_buf[BUF_SIZE] = { 0, };
    void* thread_rtn = NULL;

    int port = 443;
    char ip_addr[BUF_SIZE] = { "localhost" };
    char ca_cert_path[BUF_SIZE] = { "../certificate/rootca.crt" };

    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    connect_with_serv(client_sock, port, ip_addr);

    /* ssl_context config and create ssl */
    ssl_init();
    ctx = create_client_ssl_ctx(ca_cert_path);
    ssl = create_client_ssl(ctx, client_sock);

    do_ssl_handshake(ssl);

    /* create thread for reading socket */
    pthread_t thread_1;
    if (pthread_create(&thread_1, NULL, read_sock_t, (void*)ssl) != 0) {
        close(client_sock);
        error_handle("Failed to create thread.");
    }

    /* send message to server */
    while (1) {
        if (send_to_serv(ssl, stdin_read_buf) != 0) {
            break;
        }

        check_thread_status(&thread_1, thread_rtn);
    }

    close(client_sock);

    return 0;
}
