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
#include "lmhashtable.h"

#ifndef LM_HASHTABLE
#define LM_HASHTABLE

inline lm_hashentry* lm_hashentry_new(char Key[], void *Value){
    lm_hashentry* Hentry = malloc(sizeof(lm_hashentry));
    assert( Hentry != NULL );

    strcpy(Hentry->key, Key);
    Hentry->value = Value;
    Hentry->hash = hash_string(Key);
    Hentry->next = NULL;
    return Hentry;
}


lm_hashtable* lm_hashtable_new(){
    lm_hashtable *Table = (lm_hashtable*)malloc(sizeof(lm_hashtable));
    assert( Table != NULL );

    int i;
    for(i=0; i<MAX_BUCKETS; i++){
        Table->entries[i] = NULL;
    }
    Table->size = 0;
    return Table;
}


inline lm_hashentry*        lm_hashtable_put2(lm_hashtable *Table, lm_hashentry *Entry){
    assert( Table != NULL && Entry != NULL );
    lm_hashentry *Bucket = Table->entries[ Entry->hash % MAX_BUCKETS ];
    lm_hashentry *Aux = Bucket;

    if( Bucket == NULL ){ //empty bucket, yuppy
        Table->entries[Entry->hash % MAX_BUCKETS] = Entry;

    }else if( Bucket->hash == Entry->hash ){ //an update
        Bucket->value = Entry->value;
        #ifdef DEBUG //debug to save my sanity
            printf("\nDBG: Update on '%s'\n", Entry->key);
        #endif

    }else{ //collision, :|
        #ifdef DEBUG //debug to save my sanity
            printf("\nDBG: Collision on %s with %s\n", Entry->key, Bucket->key);
        #endif
        while( Aux->next != NULL )
            Aux = Aux->next;        
        Aux->next = Entry;
    }
    Table->size += 1;    
    return Entry;
}


inline lm_hashentry* lm_hashtable_put(lm_hashtable *Table, char Key[], void *Value){
    lm_hashentry *Entry = lm_hashentry_new(Key, Value);
    return lm_hashtable_put2(Table, Entry);
}

inline lm_hashentry* lm_hashtable_get(lm_hashtable *Table, char Key[]){
    assert( Table != NULL && Key != NULL );
    uint64_t Hash = hash_string(Key);
    lm_hashentry *Bucket = Table->entries[Hash % MAX_BUCKETS];

    if( Bucket != NULL && Bucket->hash != Hash ){ //collision
        while( Bucket != NULL && Bucket->hash != Hash )
            Bucket = Bucket->next;
    }
    return Bucket;
}

void*  lm_hashtable_free(lm_hashtable *Table){
    assert( Table != NULL );
    int i = 0;
    while( i < Table->size ){
        lm_hashentry *Entry = Table->entries[i];
        while( Entry != NULL ){
            lm_hashentry *Aux = Entry;
            Entry = Aux->next;
            free(Aux);
        }
        Table->entries[i++] = NULL;
    }
    free(Table);
    return NULL;
}


#endif
