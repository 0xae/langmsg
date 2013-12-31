#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdint.h>
#include <netdb.h>

int main(int argc, char *argv[]){
    int64_t i = 0x118163128;
    printf("%d\n", i);
    return 0;
}
