#include <SNF/network.h>

SNF_thpool *Ntwrk;

int _PORT = 9114;
int _MAX_QUEUE = 1000;

_Atomic uint64_t SNF_Total_Data_Rcv = 0;
_Atomic uint64_t SNF_Total_Data_Snt = 0;

int SNF_SERVER_SOCKET = 0;
struct sockaddr_in SNF_SERVER_ADDR;
struct sockaddr_in SNF_CLIENT_ADDR;
socklen_t SNF_CLIENT_LEN;

void *Network_Worker(void *arg);

void snf_network_init()
{
    checkerr((SNF_SERVER_SOCKET = socket(AF_INET, SOCK_STREAM, 0)), "Unable to create Socket. ");

    bzero(&SNF_SERVER_ADDR, sizeof(struct sockaddr_in));

    SNF_SERVER_ADDR.sin_family = AF_INET;
    SNF_SERVER_ADDR.sin_port = htons(_PORT);
    SNF_SERVER_ADDR.sin_addr.s_addr = htonl(INADDR_ANY);

    checkerr(bind(SNF_SERVER_SOCKET, (struct sockaddr *)&SNF_SERVER_ADDR, sizeof(SNF_SERVER_ADDR)), "Port is already taken");
    checkerr(listen(SNF_SERVER_SOCKET, _MAX_QUEUE),
             "Unable to listen in the port");

    printf("Server is now listening on PORT %d on FD %d \n", _PORT, SNF_SERVER_SOCKET);
    snf_epoll_init();
    snf_clt_init(100);
    snf_opcode_init();

    if (snf_thpool_inis(&Ntwrk, 15, Network_Worker, NULL) < 0)
    {
        fprintf(stderr, "Unable to initiate network thread pool");
        return;
    }
}
void snf_network_join()
{
    snf_thpool_join(Ntwrk);
}
void *Network_Worker(void *arg)
{
    for (;;)
    {
        snf_epoll_getList();
        for (int fd = 0; fd < SNF_NFDS; ++fd)
        {
            int sock = SNF_EPOLL_EVENTS[fd].data.fd;
            snf_epoll_del(sock);
            if (sock < SNF_SERVER_SOCKET)
            {
                continue;
            }
            if (sock == SNF_SERVER_SOCKET)
            {
                sock = accept(SNF_SERVER_SOCKET, (struct sockaddr *)&SNF_CLIENT_ADDR, &SNF_CLIENT_LEN);
                snf_thpool_addwork(Ntwrk, snf_clt_handle_new, (void *)snf_clt_new(sock));
            }
            else
            {
                snf_thpool_addwork(Ntwrk, snf_clt_handle, (void *)snf_clt_new(sock));
            }
        }
    }
    return NULL;
}

int snf_snd(SNF_CLT *Client, const char *Buffer, int _Size)
{
    int DataSnt;

    if (_Size < 0)
    {
        char buff[strlen(Buffer) + 4];
        memcpy(buff, snf_uint32_to_bytes(strlen(Buffer), 4), 4);
        strcat(buff, Buffer);
        DataSnt = send(Client->sock, buff, strlen(Buffer) + 5, 0);
    }
    else
    {
        DataSnt = send(Client->sock, Buffer, _Size, 0);
    }

    if (DataSnt == -1)
        if (errno == EPIPE)
            snf_clt_disconnect(Client);
        else
            return -1;
    else
        SNF_Total_Data_Snt += DataSnt;
    return DataSnt;
}

int snf_rcv_(SNF_CLT *Client, void *Buffer, int _Size, int _Flags)
{
    if (Client == NULL)
        return -1;

    int DataRcv = recv(Client->sock, Buffer, _Size, _Flags);
    if (DataRcv < _Size)
        if (errno == EPIPE)
            snf_clt_disconnect(Client);
        else
            return -1;
    else
        SNF_Total_Data_Rcv += DataRcv;
    return DataRcv;
}