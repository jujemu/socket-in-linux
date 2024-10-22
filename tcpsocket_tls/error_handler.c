#include <openssl/err.h>
#include <stdio.h>
#include <stdlib.h>

#define ERR_BUF_SIZE 256

void error_handle(char* msg) 
{
    printf("%s\n\n", msg);
    exit(-1);
}

void show_last_error_msg() 
{
    char msg_buf[ERR_BUF_SIZE] = { 0, };
    unsigned long err_num = ERR_get_error();

    printf("\n------ ERROR ------\n");
    printf("err_num : %ld\n", err_num);
    ERR_error_string(err_num, msg_buf);
    printf("err: %s\n", msg_buf);
}