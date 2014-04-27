#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "lmtable.h"
#include "lmlist.h"
#include "lmobject.h"

inline lmobject*  lmNewObject(int type, void *ptr){
    lmobject *obj = malloc(sizeof(lmobject));
    if(obj != NULL){
        obj->type = type;
        switch(type){
            case ATOM:
                snprintf(obj->value.atom, ATOMSIZE, (char*)ptr);
            break;
            
            case PORT:
                obj->value.port = *((int*)ptr);
            break;
            
            case PROC:
                obj->value.ptr = ptr;
            break;
            
            default:
                printf("PROGRAMMING-ERROR: BAD TYPE\n");
                exit(1);
        }
    }
    return obj;
}

lmobject* lmNewAtom(char *buf){
    return lmNewObject(ATOM, buf);
}


lmobject* lmNewProc(){
    lmlist *mailbox = lm_list_new();
    return lmNewObject(PROC, mailbox);
}


lmobject* lmNewProcWith(lmlist *mailbox){
    return lmNewObject(PROC, mailbox);
}

lmobject* lmReceive(lmobject *proc){
    lmobject *msg = NULL;
    lmlist *mailbox = proc->value.ptr;
    if(mailbox->size > 0){
        lmnode *node = lm_list_pop(mailbox);
        msg = node->content;
    }
    return msg;
}

lmobject* lmSend(lmobject *proc, lmobject *msg){
    lmlist *mailbox = proc->value.ptr;
    lmnode *node = lm_node_new(msg);
    lm_list_shift(mailbox, node);
    return msg;    
}

lmobject* lmNewPort(int fd){
    lmobject *obj = lmNewObject(PORT, &fd);
    return obj;
}

lmobject* lmWritePort(lmobject *port, lmobject *msg){
    char buff[ATOMSIZE+3];
    memset(buff, 0, ATOMSIZE);
    lmToString(msg, buff, ATOMSIZE);
    strncat(buff, "\r\n", ATOMSIZE+3);
    int written = write(port->value.port, buff, ATOMSIZE+3);
    lmobject *result;

    if(written < 0){
        char *err = strerror(errno);
        result = lmNewAtom(err);
    }else{
        snprintf(buff, ATOMSIZE, "'%d bytes'", written);
        result = lmNewAtom(buff);
    }
    return result;
}


lmobject* lmReadPort(lmobject *port){
    char buff[ATOMSIZE];
    memset(buff, 0, ATOMSIZE);
    int readen = read(port->value.port, buff, ATOMSIZE);
    lmobject *result;

    if(readen > 0){
        result = lmNewAtom(buff);
    }else{
        char *err = strerror(errno);
        result = lmNewAtom(err);
    }
    return result;
}



void lmToString(lmobject *Obj, char buff[], int bufsize){
    char *ptr;
    if(Obj == NULL){
        snprintf(buff, bufsize, "nil");
    }else if(Obj->type == ATOM){
        snprintf(buff, bufsize, "'%s'", Obj->value.atom);
    }else if(Obj->type == PORT){
        snprintf(buff, bufsize, "<port at %d>", Obj->value.port);
    }else if(Obj->type == PROC){
        lmlist *list = Obj->value.ptr;
        int len = list->size;
        snprintf(buff, bufsize, "<proc with %d messages>", len);
    }
}



void* lmFreeObject(lmobject *obj){
    if(obj->type == ATOM){
        free(obj);
    }else if(obj->type == PORT){
        close(obj->value.port);
        free(obj);
    }else if(obj->type == PROC){
        lmlist *mailbox = (lm_list*)obj->value.ptr;
        lm_list_free(mailbox);
        free(obj);
    }
    return NULL;
}


