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
#include "lmmsg.h"

#ifndef LM_MSG
#define LM_MSG

//absence of object
lm_object *nil = NULL;

lm_object *runtime = NULL;

//table of variables
lm_hashtable *symbol_table = NULL;

//vm options
char *bin_exec = NULL;
char *iname = "testing";
char *ihost = "127.0.0.1";
char *iaddr = NULL;


inline lm_object*  lm_new_object(int Type, void *Ptr){
    lm_object *Obj = malloc(sizeof(lm_object));
    assert( Obj != NULL );
    Obj->type = Type;
    Obj->ptr = Ptr;
    sprintf(Obj->repr, "<object at %p>", (unsigned long)Obj);
    return Obj;
}


inline lm_object*  lm_new_string(char *String){
    lm_object *Obj = nil;
    if( String != NULL && strcmp(String,"") != 0 ){
        char *Ptr = strdup(String);
        assert( Ptr != NULL );
        Obj = lm_new_object(STR, Ptr);    
    }
    return Obj;
}


inline lm_object*  lm_new_proc(){
    lm_list *Mailbox = lm_list_new();
    return lm_new_object(PROC, Mailbox);
}


inline lm_object*  lm_new_procwith(lm_list *Mailbox){
    return lm_new_object(PROC, Mailbox);
}

inline lm_object* lm_msg(lm_object *Obj, lm_object *Message){
    assert( Obj != NULL && Obj->type == PROC );
    if( Obj != nil && Message != nil ){
        lm_list *Mailbox = (lm_list*)Obj->ptr;
        lm_node *Node = lm_node_new(Message);
        lm_list_shift(Mailbox, Node);
    }
    return Message;
}


inline lm_object* lm_receive(lm_object *Obj){
    assert( Obj != NULL && Obj->type == PROC );
    lm_object* Message = nil;
    lm_list *Mailbox = (lm_list*)Obj->ptr;

    if( Obj != nil && Mailbox->size > 0 ){
        lm_node *Node = lm_list_pop(Mailbox);
        Message = (lm_object*)Node->content;
        free(Node);
    }

    return Message;
}


char* lm_repr(lm_object *Obj){
    if( Obj == NULL ){
        return "null";    
    }else if(Obj->type == STR){
        return (char*)Obj->ptr;
    }
    return Obj->repr;
}


inline lm_object* lm_get_var(char *Name){
    if( strcmp(Name,"nil") == 0 ){
        return nil;
    }
    lm_hashentry *Entry = lm_hashtable_get(symbol_table, Name);
    lm_object *Obj = NULL;
    if( Entry != NULL ){
        Obj = (lm_object*)Entry->value;
    }
    return Obj;
}


inline lm_object*  lm_new_procvar(char *Name){
    if( strcmp(Name,"nil") == 0 ){
        return nil;
    }
    lm_object *Proc = lm_new_proc();
    lm_hashtable_put(symbol_table, Name, Proc);
    return Proc;
}


inline lm_object* lm_new_funvar(char *Name, char **Instructions){
    return NULL;
}


void* lm_free_object(lm_object *Obj){
    if( Obj == NULL ){
        return NULL;
    }
    if( Obj->type == STR ){
        free(Obj->ptr);
    }else if( Obj->type == PROC ){
        Obj->ptr = lm_list_free((lm_list*)Obj->ptr);
    }
    free(Obj);
}


inline lm_object* lm_eval(char *Buffer){
    if( Buffer == NULL ){
        return NULL;
    }if( strcmp(Buffer,"") == 0 ){
        return nil;    
    }

    char first[60];
    char second[60];
    first[0]  = '\0';
    second[0] = '\0';

    char *ptr= first;
    char op = '\0';
    int opened = 0;
    int c,w=0;

    while( c != '\0' ){
        c = *Buffer++;
        if( c == '?' || c == '!' || c == '=' || c == '#' ){
            if( w == 0 ){
                *ptr = '\0';
                op = c;
                w += 1;
                ptr = second;
                continue;
            }
        }else if( c == ' ' || c == '\n' ){
            continue;
        }
        *ptr++ = c;
    }
    *ptr = '\0';
    
    #ifdef DEBUG
        printf("DBG: first = '%s'\n", first);
        printf("DBG: op = '%c'\n", op);
        printf("DBG: second = '%s'\n", second);
    #endif

    switch(op){
        case '?' : {
            lm_object *Obj = lm_get_var(first);
            lm_assert_exists(Obj, first);
            return lm_receive(Obj);
        }
        
        case '#' : {
            lm_object *Obj = lm_get_var(first);
            lm_assert_exists(Obj,first);

            if( Obj == nil ){
                printf("0 messages\n");
            }else{
                if( Obj->type != PROC ){
                    fprintf(stderr, "error: Invalid object '%s'\n", first);
                    return NULL;
                }
                lm_list *Mailbox = (lm_list*)Obj->ptr;
                int mailbox_size = Mailbox->size;
                printf("%d messages\n", mailbox_size);
            }

            return nil;
        }


        case '!' : {
            lm_object *Obj = lm_get_var(first);
            lm_assert_exists(Obj, first);
            lm_object *Message = NULL;
            
            if( strcmp(second,"") == 0 ){
                lm_bad_syntax(Buffer);
            }else if( second[0] == '{' ){
                int seclen = strlen(second);
                if( second[seclen-1] != '}' )
                    lm_bad_syntax(second);
                else
                    second[seclen-1] = '\0';
                Message = lm_eval(second+1);
            }else{
                Message = lm_new_string(second);
            }
            
            return lm_msg(Obj, Message);
        }


        case '=' : {
            lm_object *Obj = NULL;
            if( strcmp(second,"proc") == 0 ){
                Obj = lm_new_procvar(first);
            }else if( strcmp(second,"") == 0 ){
                lm_bad_syntax(Buffer);
            }else{
                Obj = lm_get_var(second);
                lm_assert_exists(Obj, second);
                lm_hashtable_put(symbol_table, first, Obj);
            }
            return Obj;
        }


        default : {
            if( strcmp(first,"") != 0 ){
                lm_object *Obj = lm_get_var(first);                      
                lm_assert_exists(Obj, first);

                return Obj;
            }            
            return nil;
        }
    }
}


void  lm_init(int argc, char *argv[]){
    if( argc == 2 ){
        if( strcmp(argv[1], "help") == 0 ){
            fprintf(stderr, "Usage: %s [instance_host] [instance_name]\n\n", argv[0]);
            exit(1);
        }
        ihost = argv[1];

    }else if( argc == 3 ){
        ihost = argv[1];
        iname = argv[2];

    }else if( argc > 3 ){
        fprintf(stderr, "Usage: %s [instance_host] [instance_name]\n\n", argv[0]);
        exit(1);
    }

    bin_exec = argv[0];
    nil = lm_new_object(OBJ, NULL);
    sprintf(nil->repr, "<nil at %p>", nil);
    symbol_table = lm_hashtable_new();
    lm_new_procvar("runtime");
    iaddr = malloc(sizeof(char) * 100);
    sprintf(iaddr, "%s@%s", iname, ihost);
}


void lm_terminate(){
    lm_free_object(nil);
    free(iaddr);
    iaddr = NULL;
    symbol_table = lm_hashtable_free(symbol_table);
}

#endif
