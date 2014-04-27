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
#include "lmlist.h"

#ifndef LM_LIST
#define LM_LIST

inline lm_node* lm_node_new(void *Content){
    lm_node *Node = malloc(sizeof(lm_node));
    assert(Node != NULL);
    Node->content = Content;
    Node->next = NULL;
    return Node;
}

inline lm_list* lm_list_new(){
    lm_list *List = malloc(sizeof(lm_list));
    assert( List != NULL );
    List->size = 0;
    int i;
    for(i=0; i < LIST_SIZE; i++)
        List->array[i] = NULL;
    return List;
}

inline lm_list* lm_list_shift(lm_list *List, lm_node *Node){
    assert(List != NULL && Node != NULL);
    lm_node **array = List->array;
    int i = 0;

    while(i < LIST_SIZE && array[i] != NULL)
        i += 1;
    
    if(i < LIST_SIZE){ // available space
        while(i > 0){
            array[i] = array[i-1];
            i -= 1;
        }
        List->size += 1;
    }

    array[0] = Node;
    return List;
}

inline lm_list* lm_list_add(lm_list *List, lm_node *Node){
    assert( List != NULL && Node != NULL );
    lm_node **array = List->array;

    if( List->size < LIST_SIZE ){
        array[List->size] = Node;
        List->size += 1;
    }
    
    return List;
}

inline lm_node* lm_list_get(lm_list *List, int Index){
    assert( List != NULL && Index < LIST_SIZE );
    return List->array[Index];
}

inline lm_node* lm_list_pop(lm_list *List){
    assert( List != NULL );
    lm_node **Array = List->array;
    lm_node *Node = NULL;

    if( List->size > 0 ){
        Node = Array[0];
        int i = 0;
        while(i < LIST_SIZE){
            if(Array[i] == NULL)
                break;
            Array[i] = Array[i+1];
            i += 1;
        }
        List->size -= 1;
    }
    
    return Node;
}


void  lm_list_print(lm_list *List){
    int i = 0;
    lm_node **Array = List->array;
    printf("[");
    while( i < List->size ){
        lm_node *Node = Array[i++];
        if( Node != NULL ){
            printf("%s, ", (char*)Node->content);        
        }else{
            printf("null, ");                
        }
    }

    printf("]\n");
}


void* lm_list_free(lm_list *List){
    int i = 0;
    while( i < List->size ){
        lm_node *Node = List->array[i];
        Node->next = NULL;
        List->array[i] = NULL;
        free(Node);
        i += 1;
    }
    free(List);
    return NULL;
}

#endif
