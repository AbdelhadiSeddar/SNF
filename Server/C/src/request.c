#include "SNF/request.h"

SNF_RQST *snf_request_fetch(
    SNF_CLT *Client)
{
    SNF_RQST *re = snf_request_gen();
    char *Request;

    /**
     * Fetching OPCODE
     */
    snf_rcv(Client, re->OPCODE->opcode, 4);

    /**
     * Fetching Request UID
     */
    snf_rcv(Client, re->UID, strlen(NULLREQUEST) + 1);

    /**
     * Fetching Arguments Length & handling according to it
     */
    char MsgSize[4];
    snf_rcv(Client, MsgSize, 4);
    uint32_t Size = snf_bytes_to_uint32(MsgSize, 4);

    if (Size < 0)
        return NULL;
    else if (Size > SNF_REQUEST_MAXSIZE)
        /// TODO: Handle this better
        return NULL;

    SNF_RQST_ARG *Top_rqst = NULL;
    if (Size == 0)
        goto end_request_fetchfrom_clt;
    SNF_RQST_ARG *Current = NULL;
    /**
     * Fetching Arguments
     */
    Request = calloc(Size, sizeof(char));
    snf_rcv(Client, Request, Size);
    /**
     * Separating Arguments
     */
    while (Size > 0)
    {
        int chr = (int)(strchrnul(Request, UNIT_SCEPARATOR[0]) - Request);
        char *tmpRqst = calloc(chr, sizeof(char));
        strncpy(tmpRqst, Request, chr);
        tmpRqst[chr] = '\0';
        SNF_RQST_ARG *new = snf_request_arg_gen(tmpRqst);
        if (Top_rqst == NULL)
            Top_rqst = new;
        if (Current == NULL)
            Current = new;
        else
        {
            Current->next = new;
            Current = new;
        }
        char *tmp = calloc(Size - chr - 1, sizeof(char));
        memcpy(tmp, Request + chr + 1, Size -= chr + 1);
        free(Request);
        Request = tmp;
    }
end_request_fetchfrom_clt:;
    snf_request_arg_insert(re, Top_rqst);
    return re;
}

SNF_RQST *snf_request_gen()
{
    SNF_RQST *re = calloc(1, sizeof(SNF_RQST));
    re->args = NULL;
    re->OPCODE = calloc(1, sizeof(SNF_opcode));
    return re;
}

SNF_RQST *snf_request_gen_wUID(
    const char UID[16])
{
    SNF_RQST *re = snf_request_gen();
    memcpy(re->UID, UID, 16);
    return re;
}

void snf_request_free(
    SNF_RQST *Request)
{
    if (Request == NULL)
        return;
    snf_request_args_free(Request->args);
    free(Request);
}

SNF_RQST *snf_request_gen_response(
    SNF_RQST *Original,
    SNF_opcode *OPCODE,
    SNF_RQST_ARG *Args)
{
    SNF_RQST *re = snf_request_gen_wUID(Original->UID);
    re->OPCODE = OPCODE;
    re->args = Args;
    return re;
}

SNF_RQST *snf_request_gen_server_OPCODE(
    SNF_opcode *OPCODE)
{
    return snf_request_gen_response(snf_request_gen_wUID(NULLREQUEST), OPCODE, NULL);
}

SNF_RQST *snf_request_gen_base(
    SNF_RQST *Original,
    SNF_opcode_mmbr_t Command,
    SNF_opcode_mmbr_t Detail)
{
    return snf_request_gen_response(
        Original,
        snf_opcode_get_base(Command, Detail),
        NULL);
}
SNF_RQST *snf_request_genu_base(
    SNF_RQST *Original,
    SNF_opcode_mmbr_t Command)
{
    return snf_request_gen_response(
        Original,
        snf_opcode_get_base(Command, SNF_OPCODE_BASE_DET_UNDETAILED),
        NULL);
}

int snf_request_get_nargs(
    SNF_RQST *args)
{
    if (args == NULL)
        return 0;
    int nargs = 1;
    SNF_RQST_ARG *arg = args->args;
    if (arg == NULL)
        return 0;
    while (arg->next == NULL)
    {
        nargs++;
        arg = arg->next;
    }
    return nargs;
}

SNF_RQST_ARG *snf_request_arg_gen(
    const char *arg)
{
    SNF_RQST_ARG *Arg = calloc(1, sizeof(SNF_RQST_ARG));
    Arg->arg = calloc(strlen(arg) + 1, sizeof(char));
    strcpy(Arg->arg, arg);
    return Arg;
}

void snf_request_arg_free(
    SNF_RQST_ARG *arg)
{
    if (arg == NULL)
        return;
    free(arg->arg);
    free(arg);
}

void snf_request_args_free(
    SNF_RQST_ARG *arg)
{
    if (arg == NULL)
        return;
    SNF_RQST_ARG *current = arg;
    SNF_RQST_ARG *next = current->next;
    while (next != NULL)
    {
        next = current->next;
        snf_request_arg_free(current);
        current = next;
    };
    arg = NULL;
}

void snf_request_arg_insert(
    SNF_RQST *Request,
    SNF_RQST_ARG *arg)
{
    if (Request->args == NULL)
    {
        Request->args = arg;
    }
    else
    {
        SNF_RQST_ARG *last = Request->args;
        while (last->next != NULL)
            last = last->next;
        last->next = arg;
    }
}

void snf_request_send(
    SNF_CLT *Client,
    SNF_RQST *Request)
{
    /**
     * Preparing Sending:
     * Getting Arguments Size
     */
    uint32_t Size = 0;
    SNF_RQST_ARG *args = Request->args;
    while (args)
    {
        Size += strlen(args->arg);
        args = args->next;
        if (args != NULL)
        {
            Size++;
        }
    }
    char *MsgSize;
    MsgSize = snf_uint32_to_bytes(Size, 4);
    /**
     *  Sending OPCODE
     */
    snf_snd(Client, (char *)(Request->OPCODE->opcode), 4);
    /**
     * Sending Request UID
     */
    snf_snd(Client, Request->UID, strlen(NULLREQUEST) + 1);
    /**
     * Sending Arguments Length
     */
    snf_snd(Client, MsgSize, 4);
    free(MsgSize);
    /**
     * Sending Arguments
     */
    if (Size > 0)
    {
        args = Request->args;
        while (args)
        {
            snf_snd(Client, args->arg, strlen(args->arg));
            args = args->next;
            if (args)
                snf_snd(Client, UNIT_SCEPARATOR, 1);
        }
    }
    snf_request_free(Request);
    return;
}
void snf_request_send_confirm(
    SNF_CLT *Client,
    SNF_RQST *Original)
{
    snf_request_send(Client, snf_request_genu_base(Original, SNF_OPCODE_BASE_CMD_CONFIRM));
}
void snf_request_send_reject(
    SNF_CLT *Client,
    SNF_RQST *Original)
{
    snf_request_send(Client, snf_request_genu_base(Original, SNF_OPCODE_BASE_CMD_REJECT));
}
void snf_request_send_invalid(
    SNF_CLT *Client,
    SNF_RQST *Original)
{
    snf_request_send(Client, snf_request_genu_base(Original, SNF_OPCODE_BASE_CMD_INVALID));
}
