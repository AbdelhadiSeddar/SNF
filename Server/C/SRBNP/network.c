#include "_Imports.h"

int _PORT = 9114;
int _MAX_QUEUE = 1000;

_Atomic uint64_t Total_Data_Rcv = 0;
_Atomic uint64_t Total_Data_Snt = 0;

int _SERVER_SOCKET = 0;
struct sockaddr_in _SERVER_ADDR;
struct sockaddr_in _CLIENT_ADDR;
socklen_t _CLIENT_LEN;

thpool *Ntwrk;
void *Network_Worker(void *arg);

void network_init(const char *PATH)
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
        epoll_getList();
        for (int fd = 0; fd < _NFDS; ++fd)
        {
            if (events[fd].data.fd == _SERVER_SOCKET)
            {
                clt client;
                client.sock = accept(_SERVER_SOCKET, (struct sockaddr *)&_CLIENT_ADDR, &_CLIENT_LEN);
                setnonblocking(client.sock);
                clt_handle_new(clt_new(events[fd].data.fd ));
            }
            else if (events[fd].data.fd < _SERVER_SOCKET)
                continue;
            else
            {
                // TODO handle Existing Client Using Requests
            }
        }
    }
    return NULL;
}

int snd(clt *Client, const char *Buffer, int _Size)
{
    int DataSnt;
    if (_Size < 0)
    {
        char buff[strlen(Buffer) + 5];
        strcpy(buff, fb_strlen(Buffer));
        strcat(buff, Buffer);
        DataSnt = send(Client->sock, buff, strlen(Buffer) + 5, 0);
    }
    else
    {
        DataSnt = send(Client->sock, Buffer, _Size, 0);
    }

    if (DataSnt == -1)
        if (errno == EPIPE)
            clt_disconnect(Client);
        else
            return errno;
    else
        Total_Data_Snt += DataSnt;
    return DataSnt;
}

int rcv(clt *Client, void *Buffer, int _Size)
{
    int DataRcv = recv(Client->sock, Buffer, _Size, 0);
    Total_Data_Rcv += DataRcv;
    return DataRcv;
}