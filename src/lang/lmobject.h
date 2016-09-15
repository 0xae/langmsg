#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "lmtable.h"
#include "lmlist.h"

#ifndef LM_MSG_H
#define LM_MSG_H

#define ATOMSIZE 200
#define ATOM  0x1
#define PROC  0x2
#define PORT  0x3
#define isTypeValid(type) (type >= ATOM && type <= PORT)

typedef struct{
    short type;
    struct{
        void *ptr;
        char atom[ATOMSIZE];
        int port;
    }value;
}lmobject;

lmobject* lmNewObject(int type, void *ptr);

/* Atom Api */
lmobject* lmNewAtom(char *buf);

/* Proc Api */
lmobject* lmNewProc();
lmobject* lmNewProcWith(lmlist *queue);
lmobject* lmSend(lmobject *proc, lmobject *msg);
lmobject* lmReceive(lmobject *proc);

/* Port Api */
lmobject* lmNewPort(int fd);
lmobject* lmWritePort(lmobject *port, lmobject *msg);
lmobject* lmReadPort(lmobject *port);

void*     lmFreeObject(lmobject *Obj);
void      lmToString(lmobject *lval, char buff[], int bufsize);
#endif

