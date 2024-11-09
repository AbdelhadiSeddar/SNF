#include "SNF/clt.h"

SNF_ht *SNF_Clt_ht;
void snf_clt_init(int ht_min_Size)
{
    SNF_Clt_ht = snf_hashtable_inis(ht_min_Size);
}

SNF_CLT *snf_clt_new(int Sockfd)
{
    SNF_CLT *Client = malloc(sizeof(SNF_CLT));
    pthread_mutex_init(&(Client->mutex), NULL);
    Client->sock = Sockfd;
    strcpy(Client->UUID, "00000000-0000-0000-0000-000000000000");
    return Client;
}

void snf_clt_free(SNF_CLT *Client)
{
    if (Client != NULL)
        free(Client);
}

void *snf_clt_handle_new(void *arg)
{
    if (SNF_Clt_ht == NULL)
        return NULL;

    SNF_CLT *Client = (SNF_CLT *)arg;
    checkerr(Client == NULL ? -1 : 0, "Can not handle new client: Null");
    SNF_opcode *op = calloc(1, sizeof(SNF_opcode));
    snf_rcv(Client, (char *)(uint8_t *)op->opcode, 4);
    if (snf_opcode_compare(
            op,
            snf_opcode_get_base(
                SNF_OPCODE_BASE_CMD_CONNECT,
                0x00)) >= 0)
        snf_clt_connect(Client);
    else if (snf_opcode_compare(
                 op,
                 snf_opcode_get_base(
                     SNF_OPCODE_BASE_CMD_RECONNECT,
                     0x00)) >= 0)
        snf_clt_reconnect(Client);
    else
        snf_clt_disconnect(Client);

    snf_epoll_add(SNF_SERVER_SOCKET);

    return NULL;
}

void *snf_clt_handle(void *arg)
{
    SNF_CLT *Client = (SNF_CLT *)arg;
    SNF_ht_item *Slot;
    SNF_CLT *Original;
    int i = 0;
    if ((i = snf_rcv(Client, Client->UUID, 36)) < 36 || (Slot = snf_hashtable_lookup(SNF_Clt_ht, Client->UUID)) == NULL || (Original = Slot->Content) == NULL || Original->sock != Client->sock)
    {
        snf_clt_disconnect(Client);
        goto end_clt_handle;
    }
    pthread_mutex_lock(&(Original->mutex));
    SNF_RQST *Rqst = snf_request_fetch(Client);

    if (Rqst != NULL)
    {
        SNF_RQST *re = NULL;
        if(snf_opcode_isbase(Rqst->OPCODE)
            && Rqst->OPCODE->strct.Command == SNF_OPCODE_BASE_CMD_DISCONNECT)
        {
            snf_clt_disconnect(Original);
            goto end_clt_handle;
        }
        else
        {
            SNF_opcode_LL_item *item = snf_opcode_get_command(
                Rqst->OPCODE->strct.Category,
                Rqst->OPCODE->strct.SubCategory,
                Rqst->OPCODE->strct.Command
            );
            if(item == NULL)
            {
                re = snf_cmd_invalid_unregistred(Rqst);
            }
            else if(item->func == NULL)
            {
                re = snf_cmd_invalid_unimplemented(Rqst);
            }
            else
            {
                re = item->func(Rqst);
            }
        }
        snf_request_free(Rqst);
        snf_request_send(Original, re);
    }
    pthread_mutex_unlock(&(Original->mutex));
    snf_epoll_add(Original->sock);
end_clt_handle:;
    snf_clt_free(Client);
    return NULL;
}

void snf_clt_connect(SNF_CLT *Client)
{
    uuid_t GUID;
    uuid_generate_time_safe(GUID);
    uuid_unparse_lower(GUID, Client->UUID);
    snf_hashtable_insert(SNF_Clt_ht, Client->UUID, Client);
    snf_snd(Client, Client->UUID, 36);
    snf_epoll_add(Client->sock);

}

void snf_clt_reconnect(SNF_CLT *Client)
{
    snf_rcv(Client, Client->UUID, 36);
    SNF_CLT *Original = snf_hashtable_lookup(SNF_Clt_ht, Client->UUID)->Content;
    if (Original != NULL)
    {
        if (Client->sock < SNF_SERVER_SOCKET)
        {
            Original->sock = -1;
            return;
        }
        snf_epoll_add(Client->sock);
        snf_request_send_confirm(Client, NULL);
    }
    else
    {
        snf_request_send_invalid(Client, NULL);
        snf_clt_connect(Client);
    }
}

void snf_clt_disconnect(SNF_CLT *Client)
{
    close(Client->sock);
    Client->sock = -1;
}
