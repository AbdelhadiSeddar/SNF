#ifndef hashtable_h
#define hashtable_h
#include <pthread.h>

typedef struct SRBNP_ht_item_t SRBNP_ht_item;
typedef struct SRBNP_ht_t SRBNP_ht;

struct SRBNP_ht_item_t{
    const char* Key;
    void* Content;

    SRBNP_ht_item *next;
};
struct SRBNP_ht_t{
    pthread_mutex_t     mutex;
    int                 Size;
    SRBNP_ht_item**     Contents;
};

extern SRBNP_ht * SRBNP_HashTable_Inis(int MaxItems);

extern int  SRBNP_HashTable_insert(SRBNP_ht *HashTable, const char *Key, void* Content);
extern SRBNP_ht_item *SRBNP_HashTable_lookup(SRBNP_ht *HashTable, const char *key);
extern SRBNP_ht_item *SRBNP_HashTable_delete(SRBNP_ht *HashTable, const char *key);


#endif