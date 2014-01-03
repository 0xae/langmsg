#ifndef LM_NET
#define LM_NET

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
#include <wchar.h>

#define MAXPENDING    5
#define MAX_PORTS     100
#define MAX_NODE_NAME 60+1
#define MAX_MSG_SIZE  500+1
#define BUFSIZE       500+1

typedef struct{
    int fd;
    unsigned char name[MAX_NODE_NAME];
}lm_port;


extern void die(char *error);

lm_port* ports[MAX_PORTS];
lm_port* nilport = NULL;

int len_of_string(const char *buffer){
    int i = 0;
    while( buffer[i++] != '#' )
        ;
    
    return i-1;
    
}

int get_nodename(const char *from, char *buffer){
    //skip the protocol
    int c,i=0,t=0;
    const char *from2 = from+2;
    
    while(c != '#'){
        c = from2[i];
        if( c == ':' ){
            t++;
            i++;
            continue;
        }
        if( t >= 1 ){
            break;
        }
        buffer[i++] = c;
    }
    return i;
}

void get_message(const char *from, char *buffer){
    const char *ptr = from+2; //skip the protocol
    int c=0,i=0;
    while( c != ':' && c != '#' ){
        c = ptr[i++];
    }

    if( i < BUFSIZE ){
        strcpy(buffer,from+i);    
    }
}

int last = 0;

void init_ports(){
    int i;
    for(i=0;i<MAX_PORTS;i++)
        ports[i] = nilport;
}

void register_port(char *name, int fd){
    lm_port *port = (lm_port*)malloc(sizeof(lm_port));
    assert( port != NULL );
    port->fd = fd;
    strcpy(port->name, name);
    ports[last++] = port;
}

lm_port* get_port_by_name(char *name){
    int i;
    for(i=0;i<MAX_PORTS;i++){
        lm_port *port = ports[i];
        if( port == nilport )
            continue;
        if(strcmp(port->name,name) == 0)
            return ports[i];
    }
    
    fprintf(stderr, "No such port '%s'\n", name);
    return nilport;
}

lm_port* get_port_by_fd(int fd){
    int i;
    for(i=0;i<MAX_PORTS;i++){
        lm_port *port = ports[i];
        if( port == nilport )
            continue;
        if(port->fd == fd){
            return ports[i];
        }
    }

    fprintf(stderr, "No such port %d\n", fd);
    return nilport;
}

void* free_port(lm_port *port){
    free(port);
    return nilport;
}

int lm_file_write(int fd, const char *message, int count){
    int written=0;
    while( written < count ){
        int wr = write(fd, message+written, count-written);
        if( wr == 0 ){
            break;        
        }else if( wr == -1 ){
            return -1;
        }
        written += wr;
    }
    
    return written;
}

int lm_file_read(int fd, char *message, int count){
    int received=0;
    while( received < count ){
        int rv = read(fd, message+received, count-received);
        if( rv == 0 ){
            break;        
        }else if( rv == -1 ){
            return -1;
        }
        
        received += rv;
    }
    
    return received;
}

int lm_new_socket(){
    int s, on = 1;
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "creating socket: %s", strerror(errno));
        die("at lm_new_socket()");
    }

    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
        fprintf(stderr, "setsockopt SO_REUSEADDR: %s", strerror(errno));
        die("at lm_new_socket()");
    }

    return s;
}


#endif
