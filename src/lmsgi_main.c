#include <stdio.h>  
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/poll.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <assert.h>

#include "lmeval.h"
#include "lmsgi.h"
#define MAXLINE 510
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

void lm_get_line(FILE *stream, char *buffer, int eof, int limit){
    int c;
    int keepon = 0;
    while(((c = getc(stream)) != eof) || keepon > 0){
        if(limit == 0){
            break;
        }
        if(c == '('){
            keepon += 1;
        }else if(c == ')'){
            keepon -= 1;
        }
        *buffer++ = c;
        limit -= 1;
    }
    *buffer = '\0';
}

void* event_msg(void *data){
    lmsgi_state *lmsgi = (lmsgi_state*)data;
    if(lmsgi->eport == -1){
        fprintf(stderr, "\nhidden node started\n");
        return NULL;
    }

    struct pollfd conns;
    struct pollfd *connection = &conns;
    connection->fd = lmsgi->eport;
    connection->events = POLLRDNORM;

    char buff[MAXLINE];
    lmobject *msg = NULL;
    const int max = 1;
    memset(buff, 0, MAXLINE);
    int n = 0;

    while(1){
        n = poll(connection, max, -1);
        if(n < 0){
            if(errno == EINTR){
                continue;            
            }else{
                perror("event_msg");
                close(lmsgi->eport);
                fprintf(stderr, "connection aborted\n");
            }
        }

        if(connection->revents & (POLLRDNORM | POLLERR)){
            int r = read(lmsgi->eport, buff, MAXLINE);
            if(r == 0){
                close(lmsgi->eport);
                fprintf(stderr, "connection aborted\n");
                break;
            }
            
            buff[r] = '\0';
            msg = lm_new_string(buff);
            lm_msg(lmsgi->runtime, msg); // XXX: change this
        }
    }

    return NULL;
}


void run_lmsgi(lmsgi_state *lmsgi){
    #define BUFMAX 512
    char buffer[BUFMAX];
    lmobject *Object = NULL;
    char reprbuff[REPR_BUFF_SIZE];
    pthread_t t1;
    int n;
    int cur = 0;
    pthread_create(&t1, NULL, event_msg, lmsgi);

    if(lmsgi->eport != -1){
        snprintf(buffer, BUFMAX, "@%s=runtime", lmsgi->name);
        lm_eval(lmsgi, buffer);    
    }
    memset(buffer, 0, BUFMAX);
    memset(reprbuff, 0, REPR_BUFF_SIZE);

    while(1){
        fprintf(stderr, KNRM "%s:%d> ", lmsgi->mail, lmsgi->line);
        lm_get_line(stdin, buffer, '\n', BUFMAX);
        fprintf(stderr, KRED "In: %s\n", buffer);

        if(strcmp(buffer,"exit") == 0){
            break;

        }else if(strcmp(buffer, "cookie") == 0){
            fprintf(stderr, "cookie = '%s'\n", lmsgi->cookie);

        }else if(strcmp(buffer, "help") == 0){
            fprintf(stderr,
                   "   @node ! <message>  --> send <message> to a distributed node\n"
                   "<object> ! <message>  --> send <message> to <object>\n"
                   "<object> ?            --> get last message sent to <object>\n"
                   "<object> +            --> debug <object>\n"
                   "<object>              --> get some info about <object>\n"
                   "cookie                --> print the cookie of this instance\n"
                   "<object> = proc       --> create a new object\n\n"
                   KGRN "Note: When a distributed node receives a message\n"
                        "it is stored under the @<node> where <node>\n"
                        "is the name of the node!\n");
        }else{
            Object = lm_eval(lmsgi, buffer);
            lm_repr(Object, reprbuff);
            fprintf(stderr, KMAG "%s\n", reprbuff);
        }
        lmsgi->line += 1;
    }
}


int main(int argc, char **argv){
    lmsgi_state instance;
    lm_init(&instance, argc, argv);
    run_lmsgi(&instance);
    lm_terminate(&instance, true);

    return 0;
}

