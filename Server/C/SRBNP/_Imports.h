#define _SRBNP_VER 0000
#define _GNU_SOURCE

#pragma region [Standard]
#include <errno.h>
#include <fcntl.h>
#include <netdb.h> 
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <netinet/in.h> 
#include <sys/epoll.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#pragma endregion

#pragma region [Non-Standard]
#include <uuid/uuid.h>
#include <pthread.h>
#include <math.h>
#pragma endregion

#pragma region [SRBNP Library]
#include "_BASE_OPCODE.h"
#include "clt.h"
#include "epoll.h"
#include "hashtable.h"
#include "network.h"
#include "request.h"
#include "thpool.h"
#include "utility.h"
#pragma endregion


