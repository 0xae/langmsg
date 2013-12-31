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
#include "network.h"

extern void die(char *error); //to be linked with error.o

int main(int argc, char *argv[]){
    int sock;
    struct sockaddr_in server_addr;
    struct sockaddr_in from_addr;
    unsigned short server_port;
    unsigned int from_size;

    char *server_ip;
    char *message;
    char echo_buffer[ECHOMAX+1];
    int message_len;
    int response_len;

    if( (argc<3) || (argc>4) ){
        fprintf(stderr, "Usage %s <Server Ip> <Echo Word> [<Port>]\n", argv[0]); 
        exit(1);
    }

    server_ip = argv[1];
    message = argv[2];

    message_len = strlen(message);
    if( message_len >= ECHOMAX )
        die("Message too long");

    if( argc == 4 ){
        server_port = atoi(argv[3]);    
    }else{
        server_port = 8888;    
    }

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if( sock < 0 ){
        die("socket() failed");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);

    if(sendto(sock, message, message_len, 0, 
              (struct sockaddr*)&server_addr, 
              sizeof(server_addr)) != message_len ){
          die("sendto() sent a different number of bytes");
    }
    
    from_size = sizeof(from_addr);
    response_len = recvfrom(sock, echo_buffer, ECHOMAX, 0,
                            (struct sockaddr *)&from_addr, &from_size);
    if( response_len != message_len ){
        die("recvfrom() failed");
    }

    if( server_addr.sin_addr.s_addr != from_addr.sin_addr.s_addr ){
        fprintf(stderr, "Error: packet from unknown source.\n");
        exit(1);
    }
    
    echo_buffer[response_len] = '\0';
    printf("Received %s\n", echo_buffer);

    close(sock);
    return 0;
}

