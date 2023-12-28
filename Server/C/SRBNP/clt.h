#ifndef clt_h
#define clt_h

typedef struct clt clt;

struct clt
{
    char UUID[37];
    int sock;
};

extern clt *clt_new(int Sockfd);
extern clt *clt_get_sockfd(int Sockfd);
extern clt *clt_get_uuid(const char *uuid);

extern int clt_check_sockfd(int Sockfd);
extern int clt_check_uuid(const char *uuid);

extern void clt_handle_new(clt *Client);
extern void clt_connect(clt *Client);
extern void clt_reconnect(clt *Client);
extern void clt_disconnect(clt *Client);

#endif