#ifndef clt_h
#define clt_h

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <uuid/uuid.h>
#include <SRBNP/SRBNP.h>
#include <SRBNP/opcode.h>
typedef struct SRBNP_Client_t SRBNP_CLT;

struct SRBNP_Client_t
{
    //  Mutex used to avoid Race Condition.
    pthread_mutex_t mutex;
    //  Saves the  36 ( +1 Null character ) character wide Unique User IDentifier. Used to defined the connected Client
    char UUID[37];
    //  Saves the socket id
    int sock;
};

void srbnp_clt_inis(int ht_min_Size);

extern SRBNP_CLT *srbnp_clt_new(int Sockfd);
extern void srbnp_clt_free(SRBNP_CLT *Client);
extern SRBNP_CLT *srbnp_clt_get_sockfd(int Sockfd);
extern SRBNP_CLT *srbnp_clt_get_uuid(const char *uuid);

extern int srbnp_clt_check_sockfd(int Sockfd);
extern int srbnp_clt_check_uuid(const char *uuid);

extern void *srbnp_clt_handle_new(void *arg);
extern void *srbnp_clt_handle(void *arg);

extern void srbnp_clt_connect(SRBNP_CLT *Client);
extern void srbnp_clt_reconnect(SRBNP_CLT *Client);
extern void srbnp_clt_disconnect(SRBNP_CLT *Client);

#include <SRBNP/hashtable.h>
#include <SRBNP/request.h>
#include <SRBNP/epoll.h>
#include <SRBNP/utility.h>
#include <SRBNP/network.h>
#endif