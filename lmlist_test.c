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

int main(){
    char *pt1 = "L::1";
    char *pt2 = "L::2";
    char *pt3 = "L::3";
    char *pt4 = "L::4";
    char *pt5 = "L::5";

    lm_list *List = lm_list_new();

    lm_node *N1 = lm_node_new(pt1);
    lm_node *N2 = lm_node_new(pt2);
    lm_node *N3 = lm_node_new(pt3);
    lm_node *N4 = lm_node_new(pt4);
    lm_node *N5 = lm_node_new(pt5);
    lm_node *Node = NULL;

    lm_list_shift(List, N1);
    lm_list_shift(List, N2);
    lm_list_shift(List, N3);
    lm_list_shift(List, N4);
    lm_list_shift(List, N5);

    lm_list_print(List);

    Node = lm_list_pop(List);
    assert( Node == N5 );
    lm_list_print(List);

    Node = lm_list_pop(List);
    assert( Node == N4 );
    lm_list_print(List);

    Node = lm_list_pop(List);
    assert( Node == N3 );
    lm_list_print(List);

    Node = lm_list_pop(List);
    assert( Node == N2 );
    lm_list_print(List);

    Node = lm_list_pop(List);
    assert( Node == N1 );
    lm_list_print(List);

    Node = lm_list_pop(List);
    assert( Node == NULL );
    lm_list_print(List);

    Node = lm_list_pop(List);
    assert( Node == NULL );
    lm_list_print(List);

    lm_list_shift(List, N1);
    lm_list_shift(List, N2);
    lm_list_shift(List, N3);
    lm_list_shift(List, N4);
    lm_list_shift(List, N5);

    lm_list_print(List);

    Node = lm_list_pop(List);
    assert( Node == N5 );
    lm_list_print(List);

    Node = lm_list_pop(List);
    assert( Node == N4 );
    lm_list_print(List);

    Node = lm_list_pop(List);
    assert( Node == N3 );
    lm_list_print(List);

    Node = lm_list_pop(List);
    assert( Node == N2 );
    lm_list_print(List);

    Node = lm_list_pop(List);
    assert( Node == N1 );
    lm_list_print(List);

    Node = lm_list_pop(List);
    assert( Node == NULL );
    lm_list_print(List);

    Node = lm_list_pop(List);
    assert( Node == NULL );
    lm_list_print(List);

    List = lm_list_free(List);
    assert( List == NULL );

    return 0;
}
