#include "_Imports.h"
#include "clt.h"

SRBNP_ht *SRBNP_Clt_ht;
void clt_inis(int ht_min_Size)
{
    SRBNP_Clt_ht = SRBNP_HashTable_Inis(ht_min_Size);
}

Clt *clt_new(int Sockfd)
{
    Clt *Client = malloc(sizeof(Clt));
    pthread_mutex_init(&(Client->mutex), NULL);
    Client->sock = Sockfd;
    strcpy(Client->UUID, "00000000-0000-0000-0000-000000000000");
    return Client;
}

void clt_free(Clt *Client)
{
    if (Client != NULL)
        free(Client);
}

void *clt_handle_new(void *arg)
{
    if (SRBNP_Clt_ht == NULL)
        return NULL;

    Clt *Client = (Clt *)arg;
    checkerr(Client == NULL ? -1 : 0, "Can not handle new client: Null");
    char *Buffer = calloc(4, sizeof(char));
    rcv(Client, Buffer, 4);
    if (!strcmp(Buffer, _OPCODE_CLT_CONNECT))
        clt_connect(Client);
    else if (!strcmp(Buffer, _OPCODE_CLT_RECONNECT))
        clt_reconnect(Client);
    else
        clt_disconnect(Client);

    free(Buffer);
    clt_free(Client);
    return NULL;
}

void *clt_handle(void *arg)
{
    Clt *Client = (Clt *)arg;
    Clt *Original;
    int i = 0;
    epoll_del(Client->sock);
    if ((i = rcv(Client, Client->UUID, 37)) < 37 || (Original = SRBNP_HashTable_lookup(SRBNP_Clt_ht, Client->UUID)->Content) == NULL || Original->sock != Client->sock)
    {
        clt_disconnect(Client);
        goto end_clt_handle;
    }
    pthread_mutex_lock(&(Original->mutex));
    Rqst *Rqst = request_fetchfrom_clt(Client);

    if (!strcmp(Rqst->OPCODE, _OPCODE_CLT_DISCONNECT))
        clt_disconnect(Original);
    else if (!strcmp(Rqst->OPCODE, _OPCODE_CLT_SRBNP_VER))
    {
        request_send_clt(Client,
                         request_gen_response(Rqst,
                                              _OPCODE_CLT_CONFIRM,
                                              request_arg_gen(_SRBNP_VER)));
    }
    //    else if(###)
    //    {
    //          // TODO: Handle Custom Requests
    //    }
    else
        request_send_invalid(Original, Rqst);
    pthread_mutex_unlock(&(Original->mutex));
    epoll_add(Original->sock);
    request_free(Rqst);
end_clt_handle:;
    clt_free(Client);
    return NULL;
}

void clt_connect(Clt *Client)
{
    uuid_t GUID;
    uuid_generate_time_safe(GUID);
    uuid_unparse_upper(GUID, Client->UUID);
    printf("SRBNP Client GUID for Client %d : %s\n", Client->sock, Client->UUID);
    SRBNP_HashTable_insert(SRBNP_Clt_ht, Client->UUID, Client);
    snd(Client, Client->UUID, 37);
    epoll_add(Client->sock);
}

void clt_reconnect(Clt *Client)
{
    rcv(Client, Client->UUID, 37);
    Clt *Original = SRBNP_HashTable_lookup(SRBNP_Clt_ht, Client->UUID)->Content;
    if (Original != NULL)
    {
        if (Client->sock < _SERVER_SOCKET)
        {
            Original->sock = -1;
            return;
        }
        epoll_add(Client->sock);
    }
    else
        request_send_invalid(Client, NULL);
}

void clt_disconnect(Clt *Client)
{
    pthread_mutex_lock(&(Client->mutex));
    epoll_del(Client->sock);
    close(Client->sock);
    Client->sock = -1;
    pthread_mutex_unlock(&(Client->mutex));
}
