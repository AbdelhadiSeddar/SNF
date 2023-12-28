#include "_Imports.h"

int _EPOLLFD, _NFDS;
struct epoll_event ev, events[_MAXEVENTS];
void epoll_inis()
{
    if (_SERVER_SOCKET <= 0)
        return;
    _EPOLLFD = epoll_create1(0);
    epoll_add(_SERVER_SOCKET);
}

int epoll_add(int FD)
{
    ev.events = EPOLLIN | EPOLLERR| EPOLLET;
    ev.data.fd = FD;
    if (epoll_ctl(_EPOLLFD, EPOLL_CTL_ADD, FD, &ev) == -1)
    {
        if (errno != EEXIST)
        {
            perror("epoll_ctl: listen_sock");
            exit(EXIT_FAILURE);
        }
        return 1;
    }
    return 0;
}

void epoll_del(int fd, struct epoll_event *ev)
{
    epoll_ctl(_EPOLLFD, EPOLL_CTL_DEL, fd, ev);
}

int epoll_getList()
{
    _NFDS = -1;
    while( _NFDS == -1 )
    {
        _NFDS = epoll_wait(_EPOLLFD , events, _MAXEVENTS, 10);
        if(errno != EINTR)
            return -1;
    }
    return _NFDS;
}