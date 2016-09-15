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
#include "lmtable.h"

#ifndef LM_HASHTABLE
#define LM_HASHTABLE

inline lmtentry* lm_hashentry_new(char Key[], void *Value){
    lmtentry* Hentry = malloc(sizeof(lmtentry));
    assert(Hentry != NULL);

    strcpy(Hentry->key, Key);
    Hentry->value = Value;
    Hentry->hash = hash_string(Key);
    Hentry->next = NULL;

    return Hentry;
}


lmtable* lm_hashtable_new(){
    lmtable *Table = (lmtable*)malloc(sizeof(lmtable));
    assert(Table != NULL);

    int i;
    for(i=0; i<MAX_BUCKETS; i++){
        Table->entries[i] = NULL;
    }

    Table->size = 0;
    return Table;
}


inline lmtentry*  lm_hashtable_put2(lmtable *Table, lmtentry *Entry){
    lmtentry *Bucket = Table->entries[Entry->hash % MAX_BUCKETS];
    lmtentry *Aux = Bucket;

    if(Bucket == NULL){ /* empty bucket */
        Table->entries[Entry->hash % MAX_BUCKETS] = Entry;

    }else if(Bucket->hash == Entry->hash){ /* an update */
        Bucket->value = Entry->value;
        #ifdef DEBUG //debug to save my sanity
            printf("\nDBG: Update on '%s'\n", Entry->key);
        #endif

    }else{ //collision, :|
        #ifdef DEBUG //debug to save my sanity
            printf("\nDBG: Collision on %s with %s\n", Entry->key, Bucket->key);
        #endif
        while(Aux->next != NULL)
            Aux = Aux->next;        
        Aux->next = Entry;
    }

    Table->size += 1;    
    return Entry;
}


inline lmtentry* lm_hashtable_put(lmtable *Table, char Key[], void *Value){
    lmtentry *Entry = lm_hashentry_new(Key, Value);
    return lm_hashtable_put2(Table, Entry);
}

inline lmtentry* lm_hashtable_get(lmtable *Table, char Key[]){
    uint64_t Hash = hash_string(Key);
    lmtentry *Bucket = Table->entries[Hash % MAX_BUCKETS];

    if(Bucket != NULL && Bucket->hash != Hash){ //collision
        while(Bucket != NULL && Bucket->hash != Hash)
            Bucket = Bucket->next;
    }
    return Bucket;
}

void*  lm_hashtable_free(lmtable *Table){
    int i = 0;
    while(i < Table->size){
        lm_hashentry *Entry = Table->entries[i];
        while(Entry != NULL){
            lm_hashentry *Aux = Entry;
            Entry = Aux->next;
            free(Aux);
        }
        Table->entries[i++] = NULL;
    }
    free(Table);
    return NULL;
}

void*  lm_hashtable_free_with(lmtable *Table, void (*free_entry)(lmtentry*)){
    int i = 0;
    while(i < Table->size){
        lm_hashentry *Entry = Table->entries[i];
        while(Entry != NULL){
            lm_hashentry *Aux = Entry;
            Entry = Aux->next;
            free_entry(Aux);
        }
        Table->entries[i++] = NULL;
    }
    free(Table);
    return NULL;                                         
}

#endif
