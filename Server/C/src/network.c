#include <SRBNP/network.h>

thpool *Ntwrk;

int _PORT = 9114;
int _MAX_QUEUE = 1000;

_Atomic uint64_t SRBNP_Total_Data_Rcv = 0;
_Atomic uint64_t Total_Data_Snt = 0;

int _SERVER_SOCKET = 0;
struct sockaddr_in _SERVER_ADDR;
struct sockaddr_in _CLIENT_ADDR;
socklen_t _CLIENT_LEN;

void *Network_Worker(void *arg);

void network_init()
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
    srbnp_epoll_inis();
    srbnp_clt_inis(100);

    if(srbnp_thpool_inis(&Ntwrk, 4, Network_Worker, NULL) < 0)
    {
        fprintf(stderr, "Unable to initiate network thread pool");
        return;
    }

    srbnp_thpool_join(Ntwrk);
}

void *Network_Worker(void *arg)
{
    for (;;)
    {
        srbnp_epoll_getList();
        for (int fd = 0; fd < SRBNP_NFDS; ++fd)
        {
            int sock = SRBNP_EPOLL_EVENTS[fd].data.fd;
            if (sock == _SERVER_SOCKET)
            {
                sock = accept(_SERVER_SOCKET, (struct sockaddr *)&_CLIENT_ADDR, &_CLIENT_LEN);
                srbnp_thpool_addwork(Ntwrk, srbnp_clt_handle_new, (void *)srbnp_clt_new(sock));
            }
            else if (sock < _SERVER_SOCKET)
                continue;
            else if (srbnp_handle_zombie(sock))
            {
                srbnp_thpool_addwork(Ntwrk, srbnp_clt_handle, (void *)srbnp_clt_new(sock));
            }
        }

        if (SRBNP_NFDS > 0)
        {
            printf("Dd %d\n", Ntwrk->thpool_n_works);
            srbnp_thpool_wait(Ntwrk);
        }
    }
    return NULL;
}

int srbnp_snd(SRBNP_CLT *Client, const char *Buffer, int _Size)
{
    int DataSnt;

    pthread_mutex_lock(&(Client->mutex));
    if (_Size < 0)
    {
        char buff[strlen(Buffer) + 5];
        strcpy(buff, srbnp_Fbyte_FROM_str(Buffer));
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
            srbnp_clt_disconnect(Client);
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

int srbnp_rcv_(SRBNP_CLT *Client, void *Buffer, int _Size, int _Flags)
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
            srbnp_clt_disconnect(Client);
        }
        else
        {
            return -1;
        }
    else
        SRBNP_Total_Data_Rcv += DataRcv;
    return DataRcv;
}
int srbnp_handle_zombie(int sock)
{
    char R[1];
    SRBNP_CLT *tmp = srbnp_clt_new(sock);
    if (srbnp_rcv_PEEK(tmp, R, 0) < 0)
    {
        return -1;
    }
    srbnp_clt_free(tmp);
    return 1;
}
