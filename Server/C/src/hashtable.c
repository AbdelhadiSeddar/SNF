#include "SNF/hashtable.h"

#define hash(Key) hashlittle(Key, strlen(Key), 0x592c0d1e)

SNF_ht *snf_hashtable_inis(int MaxItems)
{
    SNF_ht *re;
    if (MaxItems < 1 || (re = calloc(1, sizeof(SNF_ht))) == NULL)
        return NULL;

    pthread_mutex_init(&(re->mutex), 0);
    double l = log(MaxItems) / log(2);
    int Size = round(l);
    Size = (Size - l < 0) ? Size + 1 : Size;
    re->Size = (int)pow(2, Size);

    (re->Contents) = calloc(re->Size, sizeof(SNF_ht_item *));
    return re;
}

int snf_hashtable_insert(SNF_ht *HashTable, const char *Key, void *Content)
{
    if (HashTable == NULL || Key == NULL || Content == NULL)
        return -1;

    SNF_ht_item *new_item = calloc(1, sizeof(SNF_ht_item));
    new_item->Content = Content;
    new_item->Key = Key;
    uint32_t Hash = hash(Key);
    pthread_mutex_lock(&(HashTable->mutex));
    int pos = Hash % HashTable->Size;
    SNF_ht_item *tmp = (HashTable->Contents)[pos];
    if (tmp == NULL)
        (HashTable->Contents)[pos] = new_item;
    else
    {
        while (tmp->next != NULL)
        {
            tmp = tmp->next;
        }
        tmp->next = new_item;
    }
    pthread_mutex_unlock(&(HashTable->mutex));
    return 0;
}

SNF_ht_item *snf_hashtable_lookup(SNF_ht *HashTable, const char *Key)
{
    if (HashTable == NULL || Key == NULL)
        return NULL;

    uint32_t Hash = hash(Key);

    pthread_mutex_lock(&(HashTable->mutex));
    int pos = Hash % HashTable->Size;
    SNF_ht_item *tmp = (HashTable->Contents)[pos];
    while (tmp != NULL && strcmp(Key, tmp->Key))
        tmp = tmp->next;
    pthread_mutex_unlock(&(HashTable->mutex));
    return tmp;
}

SNF_ht_item *snf_hashtable_delete(SNF_ht *HashTable, const char *Key)
{
    if (HashTable == NULL || Key == NULL)
        return NULL;

    uint32_t Hash = hash(Key);
    pthread_mutex_lock(&(HashTable->mutex));
    int pos = Hash % HashTable->Size;
    SNF_ht_item *tmp = (HashTable->Contents)[pos];
    SNF_ht_item *deleted;
    if (tmp == NULL || (!strcmp(Key, tmp->Key) && tmp->next == NULL))
    {
        deleted = tmp;
        (HashTable->Contents)[pos] = NULL;
    }
    else
    {
        while (strcmp(Key, tmp->next->Key))
        {
            tmp = tmp->next;
            if (tmp->next == NULL)
            {
                pthread_mutex_lock(&(HashTable->mutex));
                return NULL;
            }
        }
        deleted = tmp->next;
        tmp->next = deleted->next;
    }
    pthread_mutex_unlock(&(HashTable->mutex));
    return deleted;
}