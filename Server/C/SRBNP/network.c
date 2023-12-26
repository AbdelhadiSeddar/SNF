#include "_Imports.h"

int _PORT = 9114;
int _MAX_QUEUE = 1000;

int _SERVER_SOCKET = 0;
struct sockaddr_in _SERVER_ADDR;

void network_init()
{
    checkerr( (_SERVER_SOCKET = socket(AF_INET, SOCK_STREAM, 0))
    , "Unable to create Socket. ");

    bzero(&_SERVER_ADDR, sizeof(struct sockaddr_in));

    _SERVER_ADDR.sin_family = AF_INET;
    _SERVER_ADDR.sin_port = htons(_PORT);
    _SERVER_ADDR.sin_addr.s_addr = htonl(INADDR_ANY);

    checkerr( bind(_SERVER_SOCKET, (struct sockaddr*)&_SERVER_ADDR, sizeof(_SERVER_ADDR))
    , "Port is already taken");
    checkerr(listen(_SERVER_SOCKET, _MAX_QUEUE), 
    "Unable to listen in the port");
    
    printf("Server is now listening on PORT %d on FD %d \n", _PORT, _SERVER_SOCKET);

}

void *network_worker(void*)
{

}