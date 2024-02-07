#include <SRBNP/epoll.h>

int SRBNP_EPOLLFD, SRBNP_NFDS;
struct epoll_event SRBNP_EPOLL_EVENTS[SRBNP_MAXEVENTS];
void srbnp_epoll_inis()
{
    if (_SERVER_SOCKET <= 0)
        return;
    SRBNP_EPOLLFD = epoll_create1(0);
    srbnp_epoll_add(_SERVER_SOCKET);
}

int srbnp_epoll_add(int FD)
{
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = FD;
    if (epoll_ctl(SRBNP_EPOLLFD, EPOLL_CTL_ADD, FD, &ev) == -1)
    {
        if (errno != EEXIST)
        {
            printf("Failed Operation on Socket %d\n", FD);
            perror("epoll_ctl: listen_sock");
            exit(EXIT_FAILURE);
        }
        return 1;
    }
    return 0;
}

void srbnp_epoll_del(int fd)
{
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    epoll_ctl(SRBNP_EPOLLFD, EPOLL_CTL_DEL, fd, &ev);
}

int srbnp_epoll_getList()
{
    SRBNP_NFDS = -1;
    while( SRBNP_NFDS == -1 )
    {
        SRBNP_NFDS = epoll_wait(SRBNP_EPOLLFD , SRBNP_EPOLL_EVENTS, SRBNP_MAXEVENTS, 10);
        if(errno != EINTR)
            return -1;
    }
    return SRBNP_NFDS;
}