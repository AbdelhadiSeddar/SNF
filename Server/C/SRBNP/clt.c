#include "_Imports.h"
#include "clt.h"

Clt *clt_new(int Sockfd)
{
    Clt *Client = malloc(sizeof(Clt));
    Client->sock = Sockfd;
    return Client;
}

void clt_free(Clt *Client)
{
    free(Client);
}

Clt *clt_get_sockfd(int Sockfd)
{
    return local_db_fetch_clt(_ACTION_VAR_SOCK, (void *)&Sockfd);
}

Clt *clt_get_uuid(const char *uuid)
{
    return local_db_fetch_clt(_ACTION_VAR_UUID, (void *)uuid);
}

int clt_check_sockfd(int Sockfd)
{
    return local_db_check_clt(_ACTION_VAR_SOCK, (void *)&Sockfd);
}
int clt_check_uuid(const char *uuid)
{
    return local_db_check_clt(_ACTION_VAR_UUID, (void *)uuid);
}

void *clt_handle_new(void *arg)
{
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
    char UUID[37];
    if(rcv(Client, UUID, 37) < 37 || strcmp(UUID, Client ->UUID))
    {
        clt_disconnect(Client);
        goto end_clt_handle;
    }
    Rqst *Rqst = request_fetchfrom_clt(Client);

    if (!strcmp(Rqst->OPCODE, _OPCODE_CLT_DISCONNECT))
        clt_disconnect(Client);
    else if (!strcmp(Rqst->OPCODE, _OPCODE_CLT_UPDATE))
    {
        request_send_clt(Client,
                         request_gen_response(Rqst,
                                              _OPCODE_CLT_CONFIRM,
                                              request_arg_gen(_SRBNP_VERSION)));
    }
    //    else if(###)
    //    {
    //          // TODO: Handle Custom Requests
    //    }
    else
        request_send_invalid(Client, Rqst);
    request_free(Rqst);
    end_clt_handle:;
    clt_free(Client);
    return NULL;
}

void clt_connect(Clt *Client)
{
    local_db_insert_clt(Client);
    snd(Client, Client->UUID, 37);
    epoll_add(Client->sock);
}

void clt_reconnect(Clt *Client)
{
    rcv(Client, Client->UUID, 37);
    if (clt_check_uuid(Client->UUID))
    {
        Clt *tmp = clt_get_uuid(Client->UUID);
        if (tmp->sock < _SERVER_SOCKET)
        {
            local_db_update_clt(_ACTION_VAR_SOCK, (void *)&(Client->sock), _ACTION_VAR_UUID, (void *)(Client->UUID));
            return;
        }
    }
    local_db_insert_clt(Client);
}

void clt_disconnect(Clt *Client)
{
    epoll_del(Client->sock);
    close(Client->sock);
    int i = -1;
    local_db_update_clt(_ACTION_VAR_SOCK, (void *)&(i), _ACTION_VAR_SOCK, (void *)&(Client->sock));
}
