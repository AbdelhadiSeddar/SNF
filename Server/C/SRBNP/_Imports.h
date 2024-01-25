#define _SRBNP_VER 0000
#define _GNU_SOURCE

#if __STDC_VERSION__ < 201112L || __STD_NO_ATOMICS__
#error "Atomics Not Supported!"
#else
#define HAS_ATOMICS
#endif


#pragma region [Standard]
#include <stdio.h> 
#include <errno.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <semaphore.h>
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <sys/epoll.h>
#include <uuid/uuid.h>
#pragma endregion

#pragma region [Non-Standard]
#include "utility.h"
#include "network.h"
#include "thpool.h"
#include "epoll.h"
#include "sqlite.h"
#include "clt.h"
#include "_BASE_OPCODE.h"
#include "request.h"
#pragma endregion

#pragma region [Third-party libraries]
///     SQLite Will only be used for Saving Temporary Clients info / Logs
#include "../SQLite/sqlite3.h"
#pragma endregion

