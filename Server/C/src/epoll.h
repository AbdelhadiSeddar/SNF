////////////////////////////////////////////////////////////////////
///
/// \file epoll.h
/// This file is used to define functions related to epoll 
/// that would be used mainly in network.c
///
/// \warning    Do not use any functions inside this file outside of
///             **network.c**, unless you know what you're doing.
///
/////////////////////////////////////////////////////////////////// 

#ifndef epoll_h
#define epoll_h

#include <SNF/SNF.h>
#include <SNF/network.h>

/// @brief Defines the epoll's limit 
/// @note Temporarly is a fixed value of 4096 for testing purposes
#define SNF_MAXEVENTS 4096

/// @brief Used to save the waiting ***Epoll Events***
extern struct epoll_event SNF_EPOLL_EVENTS[SNF_MAXEVENTS];
/// @brief Describes the amount of file descriptors to be handled
extern int SNF_NFDS;
/// @brief Defines Epoll's file descriptor.
extern int SNF_EPOLLFD;

/// @brief Initializes the Epoll
extern void snf_epoll_init();
/// @brief Add's a file descriptor to be handled by epoll
/// @param FD The File descriptor to be added
/// @return The result :
///         * **0** Success
///         * **1** Fail
extern int snf_epoll_add(int FD);
/// @brief removes a file descriptor from being handled by epoll
/// @param fd The File descriptor to be removed
extern void snf_epoll_del(int fd);
/// @brief Fetches the awaiting **Epoll Events**
/// @return The result of the wait
/// @note - This function will block depending on epoll_wait (set with a timeout of 10 ms)
/// @note - the awaiting **Epoll Events** will be put in \ref SNF_EPOLL_EVENTS
extern int snf_epoll_getList();

#endif