#ifndef hashtable_h
#define hashtable_h

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

#include <SNF/locate3.h>

typedef struct SNF_ht_item_t SNF_ht_item;
typedef struct SNF_ht_t SNF_ht;

struct SNF_ht_item_t
{
    const char *Key;
    void *Content;

    SNF_ht_item *next;
};
struct SNF_ht_t
{
    pthread_mutex_t mutex;
    int Size;
    SNF_ht_item **Contents;
};

extern SNF_ht *snf_hashtable_inis(int MaxItems);

extern int snf_hashtable_insert(SNF_ht *HashTable, const char *Key, void *Content);
extern SNF_ht_item *snf_hashtable_lookup(SNF_ht *HashTable, const char *key);
extern SNF_ht_item *snf_hashtable_delete(SNF_ht *HashTable, const char *key);

#endif