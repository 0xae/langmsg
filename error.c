#include <stdio.h>
#include <stdlib.h>

#ifndef LM_MSG_ERR
#define LM_MSG_ERR

void die(char *error_msg){
    perror(error_msg);
    exit(1);
}

#endif
