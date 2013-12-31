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
#include "network.h"

extern void die(char *error); //to be linked with error.o

int main(int argc, char *argv[]){
    int server_sock;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    unsigned int client_addr_len;
    char echo_buffer[ECHOMAX];
    unsigned short server_port;
    unsigned int recv_msg_size;
    
    if( argc != 2 ){
        fprintf(stderr, "Usage: %s <ServerPort>\n", argv[0]);
        exit(1);
    }
    
    server_port = atoi(argv[1]);
    server_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if( server_sock < 0 )
        die("socket() failed");
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(server_port);
    
    if(bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0  )
        die("bind() failed");
    
    while(true){
        recv_msg_size = recvfrom(server_sock, echo_buffer, ECHOMAX, 0,
                                 (struct sockaddr*)&client_addr, &client_addr_len);
        if( recv_msg_size < 0 ){
            die("recvfrom() failed");        
        }
        
        printf("Handling client %s\n", inet_ntoa(client_addr.sin_addr));
        
        int sent = sendto(server_sock, echo_buffer, recv_msg_size, 0,
                          (struct sockaddr*)&client_addr, sizeof(client_addr));
        if( sent != recv_msg_size )
            die("sendto() sent a different number of bytes");
    }
    
    return 0;
}
