#include <SRBNP/network.h>
#include "network.h"

thpool *Ntwrk;

int _PORT = 9114;
int _MAX_QUEUE = 1000;

_Atomic uint64_t SRBNP_Total_Data_Rcv = 0;
_Atomic uint64_t SRBNP_Total_Data_Snt = 0;

int SRBNP_SERVER_SOCKET = 0;
struct sockaddr_in SRBNP_SERVER_ADDR;
struct sockaddr_in SRBNP_CLIENT_ADDR;
socklen_t SRBNP_CLIENT_LEN;

void *Network_Worker(void *arg);

void srbnp_network_init()
{
    checkerr((SRBNP_SERVER_SOCKET = socket(AF_INET, SOCK_STREAM, 0)), "Unable to create Socket. ");

    bzero(&SRBNP_SERVER_ADDR, sizeof(struct sockaddr_in));

    SRBNP_SERVER_ADDR.sin_family = AF_INET;
    SRBNP_SERVER_ADDR.sin_port = htons(_PORT);
    SRBNP_SERVER_ADDR.sin_addr.s_addr = htonl(INADDR_ANY);

    checkerr(bind(SRBNP_SERVER_SOCKET, (struct sockaddr *)&SRBNP_SERVER_ADDR, sizeof(SRBNP_SERVER_ADDR)), "Port is already taken");
    checkerr(listen(SRBNP_SERVER_SOCKET, _MAX_QUEUE),
             "Unable to listen in the port");

    printf("Server is now listening on PORT %d on FD %d \n", _PORT, SRBNP_SERVER_SOCKET);
    srbnp_epoll_inis();
    srbnp_clt_inis(100);

    if (srbnp_thpool_inis(&Ntwrk, 4, Network_Worker, NULL) < 0)
    {
        fprintf(stderr, "Unable to initiate network thread pool");
        return;
    }
}
void srbnp_network_join()
{
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
            srbnp_epoll_del(sock);
            if (sock < SRBNP_SERVER_SOCKET)
            {
                continue;
            }
            if (sock == SRBNP_SERVER_SOCKET)
            {
                sock = accept(SRBNP_SERVER_SOCKET, (struct sockaddr *)&SRBNP_CLIENT_ADDR, &SRBNP_CLIENT_LEN);
                srbnp_thpool_addwork(Ntwrk, srbnp_clt_handle_new, (void *)srbnp_clt_new(sock));
            }
            else
            {
                srbnp_thpool_addwork(Ntwrk, srbnp_clt_handle, (void *)srbnp_clt_new(sock));
            }
        }

        if (SRBNP_NFDS > 0)
        {
            srbnp_thpool_wait(Ntwrk);
        }
    }
    return NULL;
}

int srbnp_snd(SRBNP_CLT *Client, const char *Buffer, int _Size)
{
    int DataSnt;

    if (_Size < 0)
    {
        char buff[strlen(Buffer) + 4];
        memcpy(buff, srbnp_uint32_to_bytes(strlen(Buffer), 4), 4);
        strcat(buff, Buffer);
        DataSnt = send(Client->sock, buff, strlen(Buffer) + 5, 0);
    }
    else
    {
        DataSnt = send(Client->sock, Buffer, _Size, 0);
    }

    if (DataSnt == -1)
        if (errno == EPIPE)
            srbnp_clt_disconnect(Client);
        else
            return -1;
    else
        SRBNP_Total_Data_Snt += DataSnt;
    return DataSnt;
}

int srbnp_rcv_(SRBNP_CLT *Client, void *Buffer, int _Size, int _Flags)
{
    if (Client == NULL)
        return -1;

    int DataRcv = recv(Client->sock, Buffer, _Size, _Flags);
    if (DataRcv < _Size)
        if (errno == EPIPE)
            srbnp_clt_disconnect(Client);
        else
            return -1;
    else
        SRBNP_Total_Data_Rcv += DataRcv;
    return DataRcv;
}