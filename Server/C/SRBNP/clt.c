#include "_Imports.h"
#include "clt.h"

clt *clt_new(int Sockfd)
{
    clt *Client = malloc(sizeof(clt));
    Client ->sock = Sockfd;
    return Client;
}

void clt_free(clt **Client)
{
    free((*Client)->UUID);
    free((*Client));
}

clt *clt_get_sockfd(int Sockfd)
{
    return local_db_fetch_clt(_ACTION_VAR_SOCK, (void*)&Sockfd);
}

clt *clt_get_uuid(const char *uuid)
{
    return local_db_fetch_clt(_ACTION_VAR_UUID, (void*)uuid);
}

int clt_check_sockfd(int Sockfd)
{
    return local_db_check_clt(_ACTION_VAR_SOCK, (void*)&Sockfd);
}
int clt_check_uuid(const char *uuid)
{
    return local_db_check_clt(_ACTION_VAR_UUID, (void*)uuid);
}

void clt_handle_new(clt *Client)
{
    char *Buffer = calloc(4, sizeof(char));
    rcv(Client, Buffer, 4);
    if(strcmp(Buffer, _OPCODE_CLT_CONNECT))
        clt_connect(Client);
    else if(strcmp(Buffer, _OPCODE_CLT_RECONNECT))
        clt_reconnect(Client);

}

void clt_handle(clt *Client)
{
    char *Buffer = calloc(4, sizeof(char));
    rcv(Client, Buffer, 4);
    if(strcmp(Buffer, _OPCODE_CLT_DISCONNECT))
        clt_disconnect(Client);
    else if(strcmp(Buffer, _OPCODE_CLT_UPDATE))
    {
        // TODO: Update Client With Appropriate Infos.
    }
//    else if(###)
//    {
//          // TODO: Handle Custom Requests
//    }
    else
    {
        // TODO: Respond Invalid
    }
}

void clt_connect(clt *Client)
{
    local_db_insert_clt(Client);
    snd(Client, Client -> UUID, 37);
    epoll_add(Client->sock);
}

void clt_reconnect(clt *Client)
{
    rcv(Client, Client->UUID, 37);
    if(clt_check_uuid(Client->UUID))
    {
        clt *tmp = clt_get_uuid(Client->UUID);
        if(tmp -> sock < _SERVER_SOCKET)
        {
            local_db_update_clt(_ACTION_VAR_SOCK, (void*)&(Client->sock), _ACTION_VAR_UUID, (void*)(Client->UUID));
            return;
        }
    }
    local_db_insert_clt(Client);
}

void clt_disconnect(clt *Client)
{
    close(Client->sock);
    int i = -1;
    local_db_update_clt(_ACTION_VAR_SOCK, (void*)&(i), _ACTION_VAR_UUID, (void*)(Client->UUID));
}
