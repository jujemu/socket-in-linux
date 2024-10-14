#include "error_handler.h"
#include <stdio.h>
#include <stdlib.h>

void error_handle(char* msg) 
{
    printf("%s\n\n", msg);
    exit(-1);
}