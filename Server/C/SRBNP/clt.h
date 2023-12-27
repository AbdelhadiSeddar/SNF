#ifndef clt_h
#define clt_h

typedef struct clt clt;

struct clt {
    char    UUID[37];
    int     sock;
};

#endif