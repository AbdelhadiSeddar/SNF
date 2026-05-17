//////////////////////////////////////////////////////////////
///
/// \file SNF/SNF.h
/// This file includes necessary Checks and includes the
/// the nesessary libraries of the library
///
/// //////////////////////////////////////////////////////////
#ifndef SNF_H
#define SNF_H

#define _SNF_HEADER_VER_MAJOR "0"
#define _SNF_HEADER_VER_MINOR "0"
#define _SNF_HEADER_VER_PATCH "3"
#define _SNF_HEADER_VER_EXTRA "-alpha"

#define _SNF_HEADER_VER       \
    _SNF_HEADER_VER_MAJOR"."  \
    _SNF_HEADER_VER_MINOR"."  \
    _SNF_HEADER_VER_PATCH     \
    _SNF_HEADER_VER_EXTRA     \


extern char* snf_getver();
extern char* snf_getsnpver();
/// @brief Defines the library's version
#define _SNF_VER snf_getver()
#define _SNF_SNP_VER snf_getsnpver()

#define _SNF "snf"

#define _GNU_SOURCE

#pragma region[Standard]
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#pragma endregion

#if __STDC_VERSION__ < 201112L
#error Use C 11 or later !
#else
#if __STDC_NO_ATOMICS__ == 1
#error Atomics not Supported, Abort.
#endif
#endif

#pragma region[Non-Standard]
#include <uuid/uuid.h>
#include <pthread.h>
#include <math.h>
#pragma endregion

#endif
