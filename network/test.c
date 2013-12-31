#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdint.h>
#include <netdb.h>
#include <pthread.h>
#include <errno.h>
#include "network.c"

int main(int argc, char *argv[]){
    char inbuffer[BUFSIZE+1];
    int rv = get_nodename("r:heelo", inbuffer);
    inbuffer[rv] = '\0';
    printf("%s\n", inbuffer);
}
