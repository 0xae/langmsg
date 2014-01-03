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

extern void die(char *error); //to be linked with error.o

int ok = 1;
int bad = 0;

void* handle_client(void *arg){
    int client_sock = *(int*)arg;
    char inbuffer[BUFSIZE+1];
    char outbuffer[BUFSIZE+1];
    char nodename[BUFSIZE-2];

    while(1){
        int received = lm_file_read(client_sock, inbuffer, BUFSIZE);
        int str_len = len_of_string(inbuffer);
        inbuffer[str_len] = '\0';
        printf("Received '%s'\n");
        
        if( received == -1 ){
            fprintf(stderr, "%d terminated abruptaly\n", client_sock);
            break;
        }

        get_nodename(inbuffer, nodename);
        int nodenamesize = len_of_string(nodename);
        nodename[nodenamesize] = '\0';

        if( strncmp(inbuffer,"r:",2) == 0 ){ // registering node
            printf("Registering node '%s'\n", nodename);
            register_port(nodename, client_sock);
            sprintf(outbuffer, "register '%s'", nodename);

        }else if ( strncmp(inbuffer, "u:", 2) == 0 ){ // unregistering node
            printf("Unregistering node '%s'\n", nodename);
            sprintf(outbuffer, "unregister '%s'", nodename);

        }else if ( strncmp(inbuffer, "a:", 2) == 0 ){ // getting the address of a node
            lm_port *port = get_port_by_name(nodename);
            int fd = -1;
            
            if( port != nilport ){
                printf("Address of node '%s' is %d\n", nodename, port->fd);
                fd = port->fd;
            }
            sprintf(outbuffer, "%d", fd);

        }else if ( strncmp(inbuffer, "m:", 2) == 0 ){
            int start = strlen(nodename)+3;
            char *message = inbuffer+start;
            lm_port *port = get_port_by_name(nodename);

            if( port != nilport ){
                strcpy(outbuffer, "ok");
                lm_file_write(port->fd, message, BUFSIZE);
            }else{
                sprintf(outbuffer, "bad_node '%s'", nodename);
            }

        }else if( strcmp(inbuffer,"quit") == 0 ){
            break;

        }else{ //malformed message
            printf("bad message '%s'\n", inbuffer);
            sprintf(outbuffer, "bad_msg '%s'", inbuffer);
        }
       
        lm_file_write(client_sock, outbuffer, BUFSIZE);
    }
    close(client_sock);
    return NULL;
}

int main(int argc, char *argv[]){
    int server_sock;
    int client_sock;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    unsigned short server_port = 8888;
    unsigned int  client_len;

    server_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if( server_sock < 0 )
        die("socket() failed");
    int on = 1;

    if(setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
        fprintf(stderr,"setsockopt SO_REUSEADDR: %s", strerror(errno));
        die("at setsockopt()");
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(server_port);

    if(bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0  ){
        die("bind() failed");
    }
    
    if(listen(server_sock, MAXPENDING) < 0){
        die("listen() failed");
    }

    char inbuffer[BUFSIZE+1];
    char outbuffer[BUFSIZE+1];
    while(true){
        printf("Server ready...\n");
        client_len = sizeof(client_addr);
        client_sock = accept(server_sock, 
                              (struct sockaddr*)&client_addr,
                                &client_len);
        if( client_sock < 0 ){
            die("accept() failed");
        }

        printf("Handling client %s\n", inet_ntoa(client_addr.sin_addr));

        pthread_t t_id;
        pthread_create(&t_id, NULL, handle_client, (void*)&client_sock);
    }
    
    close(server_sock);
    return 0;
}

