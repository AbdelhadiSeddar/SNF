#include <stdio.h> 
#include <errno.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <semaphore.h>
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <sys/epoll.h>

#pragma region [Non-Standard]
#include "utility.h"
#include "network.h"
#include "thpool.h"
#include "epoll.h"
#pragma endregion

#pragma region [Third-party libraries]
///     SQLite Will only be used for Saving Temporary Clients info / Logs
#include "../SQLite/sqlite3.h"
#pragma endregion