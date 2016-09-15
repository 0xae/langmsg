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
#include <errno.h>

#ifndef LM_LIST_H
#define LM_LIST_H

#define LIST_SIZE 30

typedef struct lm_nodep{
    void *content;
    struct lm_nodep *next;
}lm_node;


typedef struct lm_listp{
    lm_node *array[LIST_SIZE];
    int size;
}lm_list;

typedef lm_list lmlist;
typedef lm_node lmnode;

inline lm_list*  lm_list_new();
inline lm_node*  lm_node_new(void *Content);
inline lm_list*  lm_list_add(lm_list *List, lm_node *Node);
inline lm_list*  lm_list_shift(lm_list *List, lm_node *Node);
inline lm_node*  lm_list_get(lm_list *List, int Index);
inline lm_node*  lm_list_pop(lm_list *List);
void             lm_list_print(lm_list *Node);
void*            lm_list_free(lm_list *List);

inline lm_list*  lmlist_new();
inline lm_node*  lmnode_new(void *Content);
inline lm_list*  lmlist_add(lm_list *List, lm_node *Node);
inline lm_list*  lmlist_shift(lm_list *List, lm_node *Node);
inline lm_node*  lmlist_get(lm_list *List, int Index);
inline lm_node*  lmlist_pop(lm_list *List);
void             lmlist_print(lm_list *Node);
void*            lmlist_free(lm_list *List);


#endif
