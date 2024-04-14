#ifndef clt_h
#define clt_h

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <uuid/uuid.h>
#include <SNF/SNF.h>
#include <SNF/opcode.h>
typedef struct SNF_Client_t SNF_CLT;

struct SNF_Client_t
{
    //  Mutex used to avoid Race Condition.
    pthread_mutex_t mutex;
    //  Saves the  36 ( +1 Null character ) character wide Unique User IDentifier. Used to defined the connected Client
    char UUID[37];
    //  Saves the socket id
    int sock;
};

void snf_clt_init(int ht_min_Size);

extern SNF_CLT *snf_clt_new(int Sockfd);
extern void snf_clt_free(SNF_CLT *Client);
extern SNF_CLT *snf_clt_get_sockfd(int Sockfd);
extern SNF_CLT *snf_clt_get_uuid(const char *uuid);

extern int snf_clt_check_sockfd(int Sockfd);
extern int snf_clt_check_uuid(const char *uuid);

extern void *snf_clt_handle_new(void *arg);
extern void *snf_clt_handle(void *arg);

extern void snf_clt_connect(SNF_CLT *Client);
extern void snf_clt_reconnect(SNF_CLT *Client);
extern void snf_clt_disconnect(SNF_CLT *Client);

#include <SNF/hashtable.h>
#include <SNF/request.h>
#include <SNF/epoll.h>
#include <SNF/utility.h>
#include <SNF/network.h>
#endif