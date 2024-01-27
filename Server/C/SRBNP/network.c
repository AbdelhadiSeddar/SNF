#include "_Imports.h"
#include "network.h"

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
    epoll_inis();
    local_db_inis(PATH);

    inis_thpool(&Ntwrk, 4, Network_Worker, NULL);

    thpool_join(Ntwrk);
}

void *Network_Worker(void *arg)
{
    for (;;)
    {
        epoll_getList();
        Clt *Client = NULL;
        for (int fd = 0; fd < _NFDS; ++fd)
        {
            printf("Handling %d\n", events[fd].data.fd);
            int sock;
            if (events[fd].data.fd == _SERVER_SOCKET)
            {
                sock = accept(_SERVER_SOCKET, (struct sockaddr *)&_CLIENT_ADDR, &_CLIENT_LEN);
                thpool_addwork(Ntwrk, clt_handle_new, (void *)clt_new(sock));
            }
            else if (events[fd].data.fd < _SERVER_SOCKET)
                continue;
            else if (network_handle_zombie((Client = clt_get_sockfd(events[fd].data.fd))))
            {
                thpool_addwork(Ntwrk, clt_handle, (void *)Client);
            }
        }

        if (_NFDS > 0)
        {

            printf("Dd %d\n", Ntwrk->thpool_n_works);
            thpool_wait(Ntwrk);
        }
    }
    return NULL;
}

int snd(Clt *Client, const char *Buffer, int _Size)
{
    int DataSnt;

    pthread_mutex_lock(&(Client->mutex));
    if (_Size < 0)
    {
        char buff[strlen(Buffer) + 5];
        strcpy(buff, Fbyte_FROM_str(Buffer));
        strcat(buff, Buffer);
        DataSnt = send(Client->sock, buff, strlen(Buffer) + 5, 0);
    }
    else
    {
        DataSnt = send(Client->sock, Buffer, _Size, 0);
    }

    if (DataSnt == -1)
        if (errno == EPIPE)
        {
            if (pthread_mutex_trylock(&(Client->mutex)) < 0)
            {
                if (errno != EBUSY)
                {
                    perror("missuse of client mutex");
                    exit(0);
                }
            }
            pthread_mutex_unlock(&(Client->mutex));
            clt_disconnect(Client);
        }
        else
        {
            pthread_mutex_unlock(&(Client->mutex));
            return -1;
        }
    else
        Total_Data_Snt += DataSnt;
    pthread_mutex_unlock(&(Client->mutex));
    return DataSnt;
}

int rcv_(Clt *Client, void *Buffer, int _Size, int _Flags)
{
    if (Client == NULL)
        return -1;

    int DataRcv = recv(Client->sock, Buffer, _Size, _Flags);
    if (DataRcv <= _Size)
        if (errno == EPIPE)
        {
            if (pthread_mutex_trylock(&(Client->mutex)) < 0)
            {
                if (errno != EBUSY)
                {
                    perror("missuse of client mutex");
                    pthread_exit(0);
                }
            }
            pthread_mutex_unlock(&(Client->mutex));
            clt_disconnect(Client);
        }
        else
        {
            return -1;
        }
    else
        Total_Data_Rcv += DataRcv;
    return DataRcv;
}
int network_handle_zombie(Clt *Client)
{
    if (pthread_mutex_trylock(&(Client->mutex)) < 0)
        return -1;

    char R[1];
    if (Client == NULL)
        return 0;

    if (rcv_PEEK(Client, R, 0) < 0)
    {
        pthread_mutex_unlock(&(Client->mutex));
        return -1;
    }
    pthread_mutex_unlock(&(Client->mutex));
    return 1;
}
