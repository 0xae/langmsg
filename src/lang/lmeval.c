#include <stdio.h>  
#include <stdlib.h>
#include <string.h>
#include "lmobject.h"

inline lmobject* get_var(lmtable *symtable, char *name){
    lmbucket *bucket = lm_hashtable_get(symtable, name);
    lmobject *obj = NULL;
    if(bucket != NULL){
        obj = (lmobject*)bucket->value;
    }
    return obj;
}

inline lmobject* store_var(lmtable *symtable, char *name, lmobject *value){
    lmobject *obj = get_var(symtable, name);
    if(obj == NULL){
        lm_hashtable_put(symtable, name, value);
    }else{
        fprintf(stderr,"error: <cannot mutate symbol '%s'>\n", name);
    }
}


lmobject*  lm_eval(lmstate *lmsgi, char *Buffer){
    lmtable *symtable = lmsgi->symbol_table;
    if(Buffer == NULL){
        return NULL;
    }

    char first[510];
    char second[510];
    memset(first, 0, 510);
    memset(second, 0, 510);

    char *ptr= first;
    char op = '\0';
    int opened = 0;
    int c = 123 ,w=0;

    while(c != '\0'){
        c = *Buffer++;
 
        if(c == '?' || c == '!' || c == '=' || c == '#' || c == '%' || c == '+'){

            if(w == 0 && c != '%'){
                op = c;
                *ptr = '\0';
                w += 1;
                ptr = second;
            }
            
            if(c == '%' || c == '+'){ //a comment
                break;
            }
            continue;
        }else if( c == ' ' || c == '\n' ){
            continue;
        }
        *ptr++ = c;
    }
    *ptr = '\0';
    
    #ifdef DEBUG
        fprintf(stderr, "DEBUG first  = '%s'\n", first);
        fprintf(stderr, "DEBUG op     = '%c'\n", op);
        fprintf(stderr, "DEBUG second = '%s'\n", second);
    #endif
        
    switch(op){
        case '?':{
            lm_object *Obj = get_var(symtable, first);
            lm_assert_exists(Obj, first);
            return lm_receive(Obj);
        }
        
        case '%':{
            return NULL;
        }
        
        case '#':{
            lm_object *Obj = get_var(symtable, first);
            lm_assert_exists(Obj,first);

            if(Obj->type != PROC){
                fprintf(stderr, "error: Invalid object '%s'\n", first);
                return NULL;
            }
            lm_list *Mailbox = (lm_list*)Obj->value.ptr;
            int mailbox_size = Mailbox->size;
            printf("%d messages\n", mailbox_size);

            return NULL;
        }

        char *strncat(char *dest, const char *src, size_t n);

        case '!':{
            if(*first == '@'){ // distributed message passing
                char msg[1024];
                char msg2[1024];
                memset(msg, 0, 510);
                strncat(msg, first+1, 1024);
                strncat(msg, "!", 1024);
                strncat(msg, second, 1024);
                message_node(msg, msg2);
                msg_pig(lmsgi, msg2);
                return NULL;

            }else{
                lmobject *Obj = get_var(symtable, first);
                lm_assert_exists(Obj, first);
                lmobject *Message = NULL;

                if(*second == '\0'){
                    lm_bad_syntax(Buffer);

                }else if(second[0] == '{'){
                    int secondlen = strlen(second);
                    if(second[secondlen-1] != '}')
                        lm_bad_syntax(Buffer);
                    else
                        second[secondlen-1] = '\0';
                    Message = lm_eval(lmsgi, second+1);

                }else{
                    Message = lm_new_string(second);
                }
                
                return lm_msg(Obj, Message);            
            }
        }
        


        case '=':{
            lmobject *Obj = NULL;
            if(strcmp(second,"proc") == 0){
                Obj = lm_new_proc();
                store_var(symtable, first, Obj);

            }else if(strcmp(second,"") == 0){
                lm_bad_syntax(Buffer);

            }else{
                Obj = get_var(symtable, second);
                lm_assert_exists(Obj, second);
                store_var(symtable, first, Obj);
            }
            return Obj;
        }
        
        case '+':{
            char *type = "unknown";
            lmobject *Obj = get_var(symtable, first);
            lm_assert_exists(Obj, first);
            switch(Obj->type){
                case STR:
                    type = "string";
                break;

                case INT:
                    type = "int";
                break;

                case OBJ:
                    type = "obj";
                break;

                case PROC:
                    type = "proc";
                break;

                case FLOAT:
                    type = "float";
                break;
            }
            fprintf(stderr, "%s ==> %s\n", first, type);
            return NULL;
        }


        default:{
            if(*first != '\0'){
                lm_object *Obj = get_var(symtable, first);                      
                lm_assert_exists(Obj, first);

                return Obj;
            }            
            return NULL;
        }
    }
}

