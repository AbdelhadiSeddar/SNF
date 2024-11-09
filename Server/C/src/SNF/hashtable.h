//////////////////////////////////////////////////////////////
///
/// \file 
/// This file Defines the HashTable functions and macros
/// The HashTable uses lookup3.c 's hashing functions
///
/// //////////////////////////////////////////////////////////
#ifndef hashtable_h
#define hashtable_h

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

#include "lookup3.h"

/// @brief Shortened definiton of struct SNF_ht_item_t .
typedef struct SNF_ht_item_t SNF_ht_item;
/// @brief Shortened definiton of struct SNF_ht_t .
typedef struct SNF_ht_t SNF_ht;

/// @brief  Defines the structure of each element of the HashTable
struct SNF_ht_item_t
{
    /// @brief Key for indexing the HashTable Item
    const char *Key;
    /// @brief HashTable Item's Content
    void *Content;

    /// @brief Pointer of the HashTable Item in case of a collision
    SNF_ht_item *next;
};

/// @brief Defines the structure of the hashTable
/// @warning Do not try to change the \ref SNF_ht::Size of the HashTable after Initializing it.
struct SNF_ht_t
{
    pthread_mutex_t mutex;
    int Size;
    SNF_ht_item **Contents;
};

/// @brief Initializes the HashTime and allocates the needed amount
/// @param MaxItems The amount used to calculate the length (See note)
/// @return The results:
///         - **NULL** Upon failure 
///         - __SNF_ht*__ (The HashTable's pointer)
/// @note   **MaxItems** is used to count the smallest power of 2 that could fit **MaxItems**,
///         the latter would be used as the actual size of the HashTable <br><br>
///         ***Example :*** if MaxItems  == 5 , then the smallest power of 2 is **3**, and the actual size of the has table is 8 (2 to the power of **3**) <br><br>
///         * this is due to the HashTable where it is recommended the HashTable's lenghth is a power of two for best performance.
extern SNF_ht *snf_hashtable_inis(int MaxItems);

/// @brief Inserts a new Item into a HashTable.
/// @param HashTable The Hash tbale to be inserted to.
/// @param Key Item's key
/// @param Content Item's Content
/// @return The results:
///         - **-1** Upon failure
///         - **0** Upon Success
extern int snf_hashtable_insert(SNF_ht *HashTable, const char *Key, void *Content);
/// @brief Fetches (looks up) an Item from a HashTable
/// @param HashTable The HashTable to search on.
/// @param key The Item's Key
/// @return The result of the search:
///         - **NULL** if item was not found or given paramters are invalid
///         - **SNF_ht_item** (HashTable Item's pointer)
/// @warning Do not Free the resulting pointer! if you want the item and be able to free it
///          afterwards, use \ref snf_hashtable_delete instead
extern SNF_ht_item *snf_hashtable_lookup(SNF_ht *HashTable, const char *key);
/// @brief Removed an Item from a HashTable
/// @param HashTable The HashTable to search on.
/// @param key The Item's Key
/// @return The result of the deletion:
///         - **NULL** if item was not found or given paramters are invalid
///         - **SNF_ht_item** (Deleted HashTable Item's pointer)
/// @note make sure to free the returned item or else you'd get a memory leak.
extern SNF_ht_item *snf_hashtable_delete(SNF_ht *HashTable, const char *key);

#endif