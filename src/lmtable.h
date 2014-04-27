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
#include "hashstring.h"

#ifndef LM_HASHTABLE_H
#define LM_HASHTABLE_H

#define MAX_KEY_SIZE 61
#define MAX_BUCKETS  23 //the number of buckets should always
                        //be a prime and odd number
                        //to keep the number of colisions to a minimum

typedef struct lm_hashentryp{
    char     key[MAX_KEY_SIZE];
    void*    value;
    uint64_t hash;
    struct lm_hashentryp *next;
}lm_hashentry;

typedef struct lm_tablehashp{
    lm_hashentry *entries[MAX_BUCKETS];
    int size;
}lm_hashtable;

typedef lm_hashtable lmtable;
typedef lm_hashentry lmtentry;
typedef lm_hashentry lmbucket;

inline lmtentry* lm_hashentry_new(char Key[], void *Value);
lmtable*         lm_hashtable_new();
inline lmtentry* lm_hashtable_put(lmtable *Table, char Key[], void *Value);
inline lmtentry* lm_hashtable_put2(lmtable *Table, lmtentry *Entry);
inline lmtentry* lm_hashtable_get(lmtable *Table, char Key[]);
void*            lm_hashtable_free(lmtable *Table);
void*            lm_hashtable_free_with(lmtable *Table, void (*free_entry)(lmtentry*));
#endif
