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
#include <pthread.h>
#include <errno.h>

#include "lmhashtable.h"
#include "lmlist.h"

#ifndef LM_MSG_H
#define LM_MSG_H

typedef struct{
    short type;
    void  *ptr;
    char repr[35];
}lm_object;

#define OBJ  0x1
#define STR  0x2
#define INT  0x3
#define PROC 0x4
#define FUN  0x5

#define BUFSIZE    60
#define MAXPENDING 7

void               lm_init(int Argc, char *Argv[]);
void               lm_terminate();

lm_object*         lm_new_object(int Type, void *Ptr);
lm_object*         lm_new_string(char *String);
lm_object*         lm_new_proc();
lm_object*         lm_new_procwith(lm_list *Mailbox);

lm_object*         lm_msg(lm_object *Obj, lm_object *Message);
lm_object*         lm_receive(lm_object *Obj);
void*              lm_free_object(lm_object *Obj);

char*              lm_repr(lm_object *lval);
lm_object*         lm_eval(char *input);
lm_object*         lm_new_procvar(char *Name);
lm_object*         lm_new_funvar(char *Name, char **Instructions);
lm_object*         lm_get_var(char *name);


#define            lm_assert_exists(Var,Name) do{                                       \
                       if(Var == NULL ){                                                \
                           fprintf(stderr, "error: object '%s' does not exist!\n",Name);\
                           return NULL;                                                 \
                       }}while(0)


#define            lm_assert_not_null(Var) do{                                  \
                       if(Var == NULL ){                                        \
                           fprintf(stderr,                                      \
                                  "exception: null pointer on line %d in %s\n", \
                                  __LINE__, __FILE__);\                         \
                           return NULL;                                         \
                       }}while(0)


#define            lm_bad_syntax(Line) do{                                     \
                        fprintf(stderr, "error: Bad syntax at '%s'\n", Line);  \
                        return NULL;                                           \
                   }while(0)


#define            lm_bad_msg(Msg) do{                                         \
                        fprintf(stderr, "error: Bad message at '%s'\n", Msg);  \
                        return NULL;                                           \
                   }while(0)

#endif

