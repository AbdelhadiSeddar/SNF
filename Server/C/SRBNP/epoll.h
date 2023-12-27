#ifndef epoll_h
#define epoll_h

#define _MAXEVENTS 4096

extern struct epoll_event ev, events[_MAXEVENTS];
extern int _NFDS, _EPOLLFD;

extern void epoll_inis();
extern int epoll_add(int FD);
extern void epoll_del(int fd, struct epoll_event *ev);
extern int epoll_getList();

#endif