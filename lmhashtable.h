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
#define MAX_BUCKETS  21 //the number of buckets should always
                        //be a prime and odd number
                        //to reduce the number of colisions

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


inline lm_hashentry*  lm_hashentry_new(char Key[], void *Value);
lm_hashtable*         lm_hashtable_new();
inline lm_hashentry*  lm_hashtable_put(lm_hashtable *Table, char Key[], void *Value);
inline lm_hashentry*  lm_hashtable_put2(lm_hashtable *Table, lm_hashentry *Entry);
inline lm_hashentry*  lm_hashtable_get(lm_hashtable *Table, char Key[]);
void*                 lm_hashtable_free(lm_hashtable *Table);

#endif
