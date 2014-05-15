#include <stdio.h>  
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/poll.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAXLINE 1024
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define isUpper(c)  (c >= 'A' && c <= 'Z')

#include "lmtable.h"
#include "lmlist.h"
#include "lmobject.h"


/*
    The version of the shell and
    of the runtime system 
*/
const int rts_version = 1;

/*
    Symbol table 
*/
lmtable *symtable = NULL;

/* 
    Atom table, instead of atom hashes
    we store plain strings 
*/
lmtable *atomtable = NULL;


void lmInitRuntime(){
    symtable = lm_hashtable_new();
    atomtable = lm_hashtable_new();
    fprintf(stderr, "[rts] initializing...\n");
}


static lmobject* lmGetVar(char *name){
    lmtentry *entry = lm_hashtable_get(symtable, name);
    if(entry != NULL){
        return entry->value;
    }
    /* variable doesnt exist */
    return NULL;
}


static lmobject *lmGetAtom(char *name){
    lmtentry *entry = lm_hashtable_get(atomtable, name);

    if(entry == NULL){
        lmobject *atom = lmNewAtom(name);
        lm_hashtable_put(atomtable, name, atom);
        return atom;
    }else{
        return entry->value;
    }
}


static lmobject* lmStoreVar(char *name, lmobject *value){
    lm_hashtable_put(symtable, name, value);
    return value;
}


static lmobject* lmGetSafeVar(char *name){
    if(!isUpper(*name)){
        fprintf(stderr, "SyntaxError: '%s' is not a variable\n", name);
        return NULL;
    }
    lmobject *value = lmGetVar(name);
    if(value == NULL){
        fprintf(stderr, "NameError: '%s' is not bound\n", name);
    }
    return value;
}


void lmGetLine(FILE *stream, char *buffer, int eof, int limit){
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

lmobject*  lmEvalLine(char *line){
    char first[510];
    char second[510];
    char third[510];
    memset(first, 0, 510);
    memset(second, 0, 510);
    memset(third, 0, 510);

    char *ptr= first;
    int r = 0;
    int c=-1 , w=0, k=0;

    while(c != '\0'){
        c = *line++;
        if(c == '\''){
            k += 1;
            continue;
        }
        if(c == '\n'){
            continue;
        }
 
        if(c == '?' || c == '!' || c == '=' || 
            c == '+' || (c == ' ' && k%2 == 0)){
            if(w == 0 && r > 0 && c != ' '){
                r = 0;
                w = 1;
                *second = c;
                ptr = third;
            }
            continue;
        }

        if(c != ' '){
            *ptr++ = c;
            r += 1;        
        }

        if(c == '+'){
            break;
        }
    }

    #ifdef DEBUG
        fprintf(stderr, KRED "first  = '%s'\n", first);
        fprintf(stderr, KRED "second = '%s'\n", second);
        fprintf(stderr, KRED "third  = '%s'\n" KBLU, third);
    #endif

    switch(*second){
        case '?' : {
            lmobject *var = lmGetSafeVar(first);
            if(var != NULL){
                if(var->type == PROC){
                    return lmReceive(var);
                }else{
                    fprintf(stderr, "TypeError: '%s' is not an IO_Object\n", first);
                    return NULL;
                }
            }
            return var;
        }


        case '!' : {
            lmobject *var = lmGetSafeVar(first);
            lmobject *msg = lmEvalLine(third);

            if(var != NULL && msg != NULL){
                if(var->type == PROC){
                    return lmSend(var, msg);
                }else{
                    fprintf(stderr, "TypeError: '%s' is not a IO_Object\n", first);
                    return NULL;
                }
            }
            return var;
        }


        case '=' : {
            if(!isUpper(*first)){
                fprintf(stderr, "TypeError: '%s' is not a valid name\n", first);
                return NULL;
            }

            lmobject *var = lmGetVar(first);

            if(var == NULL){
                lmobject *value = lmEvalLine(third);
                lmStoreVar(first, value);
                return value;
                
            }else{
                fprintf(stderr, "TypeError: '%s' is already bound\n", first);            
                return var;
            }            
            break;
        }

        case '\0' : {
            if(!strcmp(first,"{}")){
                return lmNewProc();
            }

            if(isUpper(*first)){
                lmobject *obj = lmGetSafeVar(first);
                return obj;
            }else{
                return lmGetAtom(first);
            }
        }

        case '+' : {
            lmobject *value = lmEvalLine(first);
            if(value != NULL){
                char buff[ATOMSIZE];
                memset(buff, 0, ATOMSIZE);
                lmToString(value, buff, ATOMSIZE);
                printf("at  => %p\n", value);
                printf("val => %s\n", buff);
            }
            return value;
        }
        
        default:
            fprintf(stderr, "Invalid operator '%s'\n", second);
            return NULL;
    }
}


int main(int argc, char **argv){
    lmInitRuntime();
    char line[1024];
    char repr[1024];
    memset(line, 0, 1024);
    lmEvalLine("Runtime={}");

    while(1){
        printf("> ");
        lmGetLine(stdin, line, '\n', 1024);
        if(!strcmp(line,"exit")){
            break;        
        }else if(*line == '\0'){
            continue;
        }

        lmobject *obj = lmEvalLine(line);
        lmToString(obj, repr, 1024);
        printf("%s\n", repr);
    }

    return 0;
}











