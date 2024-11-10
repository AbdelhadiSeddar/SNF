//////////////////////////////////////////////////////////////
///
/// \file clt.h
/// This file Defines everything related to handling  Clients 
///
/// //////////////////////////////////////////////////////////

#ifndef clt_h
#define clt_h

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <uuid/uuid.h>
#include <SNF/SNF.h>

/// @brief Shortened definiton of struct SNF_Client_t .
typedef struct SNF_Client_t SNF_CLT;

/// @brief The structure for each saved client.
struct SNF_Client_t
{
    /// @brief Mutex used to avoid Race Condition.
    pthread_mutex_t mutex;
    /// @brief Saves the 36 ( +1 Null character ) character wide Unique User IDentifier. Used to defined the connected Client
    char UUID[37];
    /// @brief Saves the socket id
    int sock;
    /// @brief Additional custom client data.
    void *data;
};

#include <SNF/opcode.h>
/// @brief Initialises the HashTable that saves the clients
/// @param ht_min_Size The HashTable's length ( See note )
/// @param client_data_size Sets the size of the additional client data's structure.
///
/// @note   **ht_min_Size** isn't (in most cases) the same as the true size of HashTable length \line
/// @note   \line See \ref snf_hashtable_inis() .
/// 
/// 
/// @note   **client_data_size** is recommended to have the value of sizeof(YOUR_CLIENT_DATA_STRUCT)
extern void snf_clt_init(
    int ht_min_Size,
    size_t client_data_size
);

/// @brief  Creates (and allocates) a new Client using their **Sockfd**, 
///         with a default \ref SNF_CLT::UUID with the value of "00000000-0000-0000-0000-000000000000"
/// @param Sockfd The Client's socket file descriptor socket's file descriptor
/// @return the new Instance. 
/// @note Free with snf_clt_free
extern SNF_CLT *snf_clt_new(int Sockfd);

/// @brief Frees the allocated \ref SNF_CLT
/// @param Client The SNF_CLT instance to be free'd.
/// @warning Make sure you closed the socket file discriptor if you have no use for it.
extern void snf_clt_free(SNF_CLT *Client);
/// @brief Searches for a SNF_CLT depending on their socket file discriptor
/// @param Sockfd The socket descriptor to search with
/// @return SNF_CLT address pointer where it's value could be:
///         * The searched SNF_CLT instance's address.<br>
///         * **NULL** if no SNF_CLT instance found.<br>
/// @warning    NOT YET IMPLEMENTED. DO NOT USE.
extern SNF_CLT *snf_clt_get_sockfd(int Sockfd);
/// @brief Searches for a  SNF_CLT depending on their UUID
/// @param uuid The socket descriptor to search with
/// @return SNF_CLT address pointer where it's value could be:
///         * The searched SNF_CLT instance's address.<br>
///         * **NULL** if no SNF_CLT found.<br>
/// @warning    NOT YET IMPLEMENTED. DO NOT USE.
extern SNF_CLT *snf_clt_get_uuid(const char *uuid);

/// @brief Checks the existance of a SNF_CLT depending on their socket's file descriptor
/// @param Sockfd The socket descriptor to search with
/// @return An Integer dipicting the check's result:
///         * **1** if found<br>
///         * **0** if not found.<br>
/// @warning    NOT YET IMPLEMENTED. DO NOT USE.
extern int snf_clt_check_sockfd(int Sockfd);
/// @brief Checks the existance of a SNF_CLT depending on their \ref SNF_CLT::UUID
/// @param uuid The socket descriptor to search with
/// @return An Integer dipicting the check's result:
///         * **1** if found<br>
///         * **0** if not found.<br>
/// @warning    NOT YET IMPLEMENTED. DO NOT USE.
extern int snf_clt_check_uuid(const char *uuid);

/// @brief Handles new incoming Clients.
/// @param arg new Clitent's address ( SNF_CLT pointer) 
/// @return **NULL**
/// @warning  Core Function in network.c do not use it elsewhere.
extern void *snf_clt_handle_new(void *arg);
/// @brief Handles existing Clients.
/// @param arg new Clitent's address ( SNF_CLT pointer) 
/// @return **NULL**
/// @warning  Core Function in network.c do not use it elsewhere.
extern void *snf_clt_handle(void *arg);

/// @brief Function to be called upon Client Connection.
/// @param Client Connecting Client( SNF_CLT pointer) 
/// @warning  Core Function in clt.c do not use it elsewhere.
extern void snf_clt_connect(SNF_CLT *Client);
/// @brief Function to be called upon Client Re-Connection
/// @param Client Re-Connecting Client 
/// @warning  Core Function in clt.c do not use it elsewhere.
extern void snf_clt_reconnect(SNF_CLT *Client);
/// @brief Function to be called upon Client Disconnection
/// @param Client Disconnecting Client.
/// @warning  Core Function in clt.c do not use it elsewhere.
extern void snf_clt_disconnect(SNF_CLT *Client);

#include <SNF/hashtable.h>
#include <SNF/request.h>
#include <SNF/epoll.h>
#include <SNF/utility.h>
#include <SNF/network.h>
#include <SNF/cmds.h>
#endif