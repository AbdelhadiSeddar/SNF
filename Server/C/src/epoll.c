#include "SNF/epoll.h"

int SNF_EPOLLFD, SNF_NFDS;
void snf_epoll_init()
{
    if (SNF_SERVER_SOCKET <= 0 || !*snf_var_get(SNF_VAR_INITIALIZED, int))
        exit(EXIT_FAILURE);
    SNF_EPOLLFD = epoll_create1(0);
    snf_epoll_add(SNF_SERVER_SOCKET);
}

int snf_epoll_add(int FD)
{
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = FD;
    if (epoll_ctl(SNF_EPOLLFD, EPOLL_CTL_ADD, FD, &ev) == -1)
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

void snf_epoll_del(int fd)
{
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    epoll_ctl(SNF_EPOLLFD, EPOLL_CTL_DEL, fd, &ev);
}

int snf_epoll_getList()
{
    SNF_NFDS = -1;
    while (SNF_NFDS == -1)
    {
        SNF_NFDS = epoll_wait(
            SNF_EPOLLFD,
            snf_var_get(SNF_VAR_EPOLL_EVENTS, struct epoll_event),
            *snf_var_get(SNF_VAR_EPOLL_MAXEVENTS, int),
            *snf_var_get(SNF_VAR_EPOLL_TIMEOUT, _Atomic int));
        if (errno != EINTR)
            return -1;
    }
    return SNF_NFDS;
}