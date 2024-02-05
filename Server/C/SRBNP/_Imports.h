#define _SRBNP_VER 0000
#define _GNU_SOURCE

#pragma region [Standard]
#include <stdio.h> 
#include <errno.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <semaphore.h>
#include <math.h>
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
#include "_BASE_OPCODE.h"
#include "clt.h"
#include "epoll.h"
#include "hashtable.h"
#include "network.h"
#include "request.h"
#include "thpool.h"
#include "utility.h"
#pragma endregion


