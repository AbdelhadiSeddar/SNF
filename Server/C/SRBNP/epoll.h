#ifndef epoll_h
#define epoll_h

#define SRBNP_MAXEVENTS 4096

extern struct epoll_event SRBNP_EPOLL_EVENTS[SRBNP_MAXEVENTS];
extern int SRBNP_NFDS, SRBNP_EPOLLFD;

extern void srbnp_epoll_inis();
extern int srbnp_epoll_add(int FD);
extern void srbnp_epoll_del(int fd);
extern int srbnp_epoll_getList();

#endif