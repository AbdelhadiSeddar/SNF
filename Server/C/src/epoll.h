#ifndef epoll_h
#define epoll_h

#include <SNF/SNF.h>
#include <SNF/network.h>

#define SNF_MAXEVENTS 4096

extern struct epoll_event SNF_EPOLL_EVENTS[SNF_MAXEVENTS];
extern int SNF_NFDS, SNF_EPOLLFD;

extern void snf_epoll_init();
extern int snf_epoll_add(int FD);
extern void snf_epoll_del(int fd);
extern int snf_epoll_getList();

#endif