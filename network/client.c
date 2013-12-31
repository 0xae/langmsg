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
#include <errno.h>
#include "network.c"

extern void die(char *error); //to be linked with error.o
lm_port vmport;

void* mailbox_proc(void *data){
    int *sock = (int*)data;
    char mailbox[BUFSIZE+1];
    while(1){
        int rv = lm_file_read(*sock, mailbox, BUFSIZE);
        mailbox[rv] = '\0';
        printf("\n%s\n", mailbox);
    }

    return NULL;
}

int main(int argc, char *argv[]){
    int sock;
    struct sockaddr_in server_addr;
    unsigned short server_port = 8888;
    char *server_ip = "127.0.0.1";
    char *name = (argc <= 1) ? "testing" : argv[1];
    pthread_t mailbox_thread;

    sock=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if( sock < 0 )
        die("socket() failed");
    int on = 1;
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
        fprintf(stderr,"setsockopt SO_REUSEADDR: %s", strerror(errno));
        die("at setsockopt()");
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);
    int conn = connect(sock,
                       (struct sockaddr*)&server_addr,
                       sizeof(struct sockaddr_in));
    if( conn < 0 ){
        die("connect() failed");        
    }

    vmport.fd = sock;
    strcpy(vmport.name, name);
    
    pthread_create(&mailbox_thread, NULL, mailbox_proc, (void*)&sock);

    char message[BUFSIZE+1];
    char buffer[BUFSIZE+1];
    char in_buffer[BUFSIZE];

    while(1){
        printf("%s@%d> ", vmport.name,vmport.fd);
        gets(message);
        unsigned int message_len = strlen(message);
        
        if( message_len == 0 ){
            continue;
        }
        if( message_len >= BUFSIZE ){
            fprintf(stderr,"Message too long, max is %d\n", BUFSIZE);
            continue;
        }
            
        message[message_len] = '\0';
        lm_file_write(sock, message, BUFSIZE);

        if( strcmp(message, "quit") == 0 ){
            break;
        }

    }
 
    close(sock);
    return 0;
}
