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
#include <readline/readline.h>
#include <readline/history.h>
#include "lmlist.h"
#include "lmhashtable.h"
#include "lmmsg.h"

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

extern char *bin_exec;
extern char *iname;
extern char *ihost;
extern char *iaddr;


void lm_get_line(FILE *stream, char *buffer, int eof, int limit){
    int c;
    int keepon = 0;
    while( ((c = getc(stream)) != eof) || keepon > 0 ){
        if( limit == 0 ){
            break;
        }
        if( c == '{' ){
            keepon += 1;
        }else if( c == '}' ){
            keepon -= 1;
        }
        *buffer++ = c;
        limit -= 1;
    }
    *buffer = '\0';
}


int main(int argc, char *argv[]){
    lm_init(argc, argv);

    printf("lmsg is a small tool for "
            "distributed node comunication\n"
            "Type help for an introduction or quit for exiting\n"
            "bin_exec=%s\n"
            "instance_name=%s\n"
            "instance_host=%s\n"
            "instance_addr=%s\n\n", bin_exec, iname, ihost, iaddr);

    char buffer[512];
    do{
        printf(KNRM "%s> ", iaddr);
        lm_get_line(stdin, buffer, '\n', 510);
        printf(KRED "In: '%s'\n", buffer);

        if( strcmp(buffer,"quit") == 0 ){
            break;
        }else if( strcmp(buffer, "help") == 0){
            //TODO: Help manual over here
        }else{
            lm_object *Object = lm_eval(buffer);
            char* Repr = lm_repr(Object);
            printf(KMAG "%s\n", Repr);
        }
    }while(1); // conditions on do-while
               // confuses me a lot
    lm_terminate();
    return 0;
}
