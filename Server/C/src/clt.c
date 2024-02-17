#include <SRBNP/clt.h>

SRBNP_ht *SRBNP_Clt_ht;
void srbnp_clt_inis(int ht_min_Size)
{
    SRBNP_Clt_ht = srbnp_hashtable_inis(ht_min_Size);
}

SRBNP_CLT *srbnp_clt_new(int Sockfd)
{
    SRBNP_CLT *Client = malloc(sizeof(SRBNP_CLT));
    pthread_mutex_init(&(Client->mutex), NULL);
    Client->sock = Sockfd;
    strcpy(Client->UUID, "00000000-0000-0000-0000-000000000000");
    return Client;
}

void srbnp_clt_free(SRBNP_CLT *Client)
{
    if (Client != NULL)
        free(Client);
}

void *srbnp_clt_handle_new(void *arg)
{
    if (SRBNP_Clt_ht == NULL)
        return NULL;

    SRBNP_CLT *Client = (SRBNP_CLT *)arg;
    checkerr(Client == NULL ? -1 : 0, "Can not handle new client: Null");
    SRBNP_opcode *op = calloc(1, sizeof(SRBNP_opcode));
    srbnp_rcv(Client, (char*)(uint8_t*)op->opcode, 4);
    if (srbnp_opcode_compare(
            op,
            srbnp_opcode_get_base(
                SRBNP_OPCODE_BASE_CMD_CONNECT,
                0x00)) >= 0)
        srbnp_clt_connect(Client);
    else if (srbnp_opcode_compare(
                 op,
                 srbnp_opcode_get_base(
                     SRBNP_OPCODE_BASE_CMD_RECONNECT,
                     0x00)) >= 0)
        srbnp_clt_reconnect(Client);
    else
        srbnp_clt_disconnect(Client);

    srbnp_epoll_add(SRBNP_SERVER_SOCKET);

    srbnp_clt_free(Client);
    return NULL;
}

void *srbnp_clt_handle(void *arg)
{
    SRBNP_CLT *Client = (SRBNP_CLT *)arg;
    SRBNP_ht_item *Slot;
    SRBNP_CLT *Original;
    int i = 0;
    if ((i = srbnp_rcv(Client, Client->UUID, 37)) < 37 || (Slot = srbnp_hashtable_lookup(SRBNP_Clt_ht, Client->UUID)) == NULL || (Original = Slot->Content) == NULL || Original->sock != Client->sock)
    {
        srbnp_clt_disconnect(Client);
        goto end_clt_handle;
    }
    pthread_mutex_lock(&(Original->mutex));
    SRBNP_RQST *Rqst = srbnp_request_fetchfrom_clt(Client);

    if (Rqst != NULL)
    {
        printf("Request : %x\n", Rqst->OPCODE->strct.Command);
        if (srbnp_opcode_compare(
                Rqst->OPCODE,
                srbnp_opcode_get_base(
                    SRBNP_OPCODE_BASE_CMD_DISCONNECT,
                    0x00)) >= 0)
            srbnp_clt_disconnect(Original);
        else if (srbnp_opcode_compare(
                     Rqst->OPCODE,
                     srbnp_opcode_get_base(
                         SRBNP_OPCODE_BASE_CMD_CONNECT,
                         0x00)) >= 0)
        {
            // srbnp_request_send_clt(Client,
            //                        srbnp_request_gen_response(Rqst,
            //                                                   _OPCODE_CLT_CONFIRM,
            //                                                   srbnp_request_arg_gen(_SRBNP_VER)));
        }
        //    else if(###)
        //    {
        //          // TODO: Handle Custom Requests
        //    }
        else
            //            srbnp_request_send_invalid(Original, Rqst);
            printf("Invalid Request Handle");
    }
    else
        printf("Invalid request\n");
    pthread_mutex_unlock(&(Original->mutex));
    srbnp_epoll_add(Original->sock);
    srbnp_request_free(Rqst);
end_clt_handle:;
    srbnp_clt_free(Client);
    return NULL;
}

void srbnp_clt_connect(SRBNP_CLT *Client)
{
    uuid_t GUID;
    uuid_generate_time_safe(GUID);
    uuid_unparse_lower(GUID, Client->UUID);
    srbnp_hashtable_insert(SRBNP_Clt_ht, Client->UUID, Client);
    srbnp_snd(Client, Client->UUID, 37);
    srbnp_epoll_add(Client->sock);

    printf("Client %s Connected (%d)\n", Client->UUID, Client->sock);
}

void srbnp_clt_reconnect(SRBNP_CLT *Client)
{
    srbnp_rcv(Client, Client->UUID, 37);
    SRBNP_CLT *Original = srbnp_hashtable_lookup(SRBNP_Clt_ht, Client->UUID)->Content;
    if (Original != NULL)
    {
        if (Client->sock < SRBNP_SERVER_SOCKET)
        {
            Original->sock = -1;
            return;
        }
        srbnp_epoll_add(Client->sock);
    }
    else
        srbnp_request_send_invalid(Client, NULL);
}

void srbnp_clt_disconnect(SRBNP_CLT *Client)
{
    close(Client->sock);
    Client->sock = -1;
}
