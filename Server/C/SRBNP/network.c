#include "_Imports.h"

int _PORT = 9114;
int _MAX_QUEUE = 1000;

int _SERVER_SOCKET = 0;
struct sockaddr_in _SERVER_ADDR;

thpool* Ntwrk;
void *Network_Worker(void *arg);

void network_init(const char* PATH)
{
    checkerr((_SERVER_SOCKET = socket(AF_INET, SOCK_STREAM, 0)), "Unable to create Socket. ");

    bzero(&_SERVER_ADDR, sizeof(struct sockaddr_in));

    _SERVER_ADDR.sin_family = AF_INET;
    _SERVER_ADDR.sin_port = htons(_PORT);
    _SERVER_ADDR.sin_addr.s_addr = htonl(INADDR_ANY);

    checkerr(bind(_SERVER_SOCKET, (struct sockaddr *)&_SERVER_ADDR, sizeof(_SERVER_ADDR)), "Port is already taken");
    checkerr(listen(_SERVER_SOCKET, _MAX_QUEUE),
             "Unable to listen in the port");

    printf("Server is now listening on PORT %d on FD %d \n", _PORT, _SERVER_SOCKET);
    printf("Using SQLite %s\n", sqlite3_libversion()); 
    epoll_inis();
    local_db_inis(PATH);
    
    Ntwrk = inis_thpool(4, Network_Worker, NULL);

    thpool_join(Ntwrk);
}

void *Network_Worker(void *arg)
{
    for (;;)
    {
        checkerr(epoll_wait(_EPOLLFD, events, _MAXEVENTS, -1), "Waiting Failed");

        for (int fd = 0; fd < _NFDS; ++fd)
        {
            if (events[fd].data.fd == _SERVER_SOCKET)
            {
                // TODO Handle new Connections
            }
            else if (events[fd].data.fd < _SERVER_SOCKET)
                continue;
            else
            {
                clt test;
                test.sock = events[fd].data.fd;
                local_db_insert_clt(&test);
            }
        }
    }
    return NULL;
}