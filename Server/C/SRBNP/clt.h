#ifndef clt_h
#define clt_h

#define NULLREQUEST "000000000000000"
typedef struct Client_t Clt;

struct Client_t
{
    char UUID[37];
    int sock;
};

extern Clt *clt_new(int Sockfd);
extern void clt_free(Clt *Client);
extern Clt *clt_get_sockfd(int Sockfd);
extern Clt *clt_get_uuid(const char *uuid);

extern int clt_check_sockfd(int Sockfd);
extern int clt_check_uuid(const char *uuid);

extern void *clt_handle_new(void *arg);
extern void *clt_handle(void *arg);

extern void clt_connect(Clt *Client);
extern void clt_reconnect(Clt *Client);
extern void clt_disconnect(Clt *Client);

#endif